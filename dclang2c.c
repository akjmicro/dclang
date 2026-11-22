/* Host-side compiler for bounded stereo DSP words. See README.md. */
#define _GNU_SOURCE
#include <stdlib.h>

static unsigned long setup_draws;
static int syntax_errors;

static int compiler_rand(void)
{
    ++setup_draws;
    return rand();
}

#define rand compiler_rand
#define DCLANG_SYNTAX_ERROR() (++syntax_errors)
#include "libdclang.c"
#undef DCLANG_SYNTAX_ERROR
#undef rand
#include <stdarg.h>

static FILE *output;
static unsigned int next_scope;
static const unsigned int seed = 12345;
static const char *current_word;
static DCLANG_PTR current_pc;

static void emit(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
}

static void fail(const char *message)
{
    fprintf(stderr, "dclang2c: %s at instruction %zu: %s\n",
            current_word, (size_t)current_pc, message);
    exit(EXIT_FAILURE);
}

static void require_depth(int depth, int needed)
{
    if (depth < needed)
        fail("stack underflow in audio word");
}

static int emit_word(DCLANG_PTR start, int depth, unsigned int nesting);

static int emit_range(DCLANG_PTR start, DCLANG_PTR end, int depth,
                      unsigned int scope, unsigned int nesting, const char *name)
{
    for (DCLANG_PTR pc = start; pc < end; ++pc) {
        current_word = name;
        current_pc = pc;
        inst_struct instruction = prog[pc];
        const char *binary = NULL;
        const char *unary = NULL;
        int target;
        switch (instruction.opcode) {
        case OP_PUSH:
        case OP_PUSH_NO_CHECK:
            if (!isfinite(instruction.param))
                fail("non-finite literal");
            emit("s[%d] = %a;\n", depth++, instruction.param);
            break;
        case OP_GET_LOCAL:
            emit("s[%d] = local%u_%d;\n", depth++, scope, (int)instruction.param);
            break;
        case OP_SET_LOCAL:
            require_depth(depth, 1);
            emit("local%u_%d = s[%d];\n", scope, (int)instruction.param, --depth);
            break;
        case OP_CALL:
            depth = emit_word((DCLANG_PTR)instruction.param, depth, nesting + 1);
            break;
        case OP_JUMPZ: {
            require_depth(depth, 1);
            target = (int)instruction.param;
            if (target <= (int)pc || target > (int)end)
                fail("unsupported branch target");
            emit("if ((long)s[%d]) {\n", --depth);
            if (prog[target - 1].opcode == OP_JUMPU) {
                int join = (int)prog[target - 1].param;
                if (join < target || join > (int)end)
                    fail("unsupported else target");
                int left = emit_range(pc + 1, target - 1, depth, scope, nesting, name);
                emit("} else {\n");
                int right = emit_range(target, join, depth, scope, nesting, name);
                if (left != right)
                    fail("branches leave different stack depths");
                depth = left;
                pc = join - 1;
            } else {
                if (emit_range(pc + 1, target, depth, scope, nesting, name) != depth)
                    fail("conditional changes depth without an else branch");
                pc = target - 1;
            }
            emit("}\n");
            break;
        }
        case OP_DROP: require_depth(depth, 1); --depth; break;
        case OP_DROP2: require_depth(depth, 2); depth -= 2; break;
        case OP_DUP:
        case OP_OVER:
            target = instruction.opcode == OP_DUP ? 1 : 2;
            require_depth(depth, target);
            emit("s[%d] = s[%d];\n", depth, depth - target);
            ++depth;
            break;
        case OP_DUP2:
            require_depth(depth, 2);
            emit("s[%d] = s[%d]; s[%d] = s[%d];\n",
                 depth, depth - 2, depth + 1, depth - 1);
            depth += 2;
            break;
        case OP_SWAP:
            require_depth(depth, 2);
            emit("{ double a = s[%d]; s[%d] = s[%d]; s[%d] = a; }\n",
                 depth - 2, depth - 2, depth - 1, depth - 1);
            break;
        case OP_INC:
            require_depth(depth, 1);
            emit("s[%d] += 1.0;\n", depth - 1);
            break;
        case OP_PEEK:
            require_depth(depth, 1);
            emit("s[%d] = *cell(s[%d]);\n", depth - 1, depth - 1);
            break;
        case OP_PEEKOFFSET:
            require_depth(depth, 2);
            --depth;
            emit("s[%d] = *offset_cell(s[%d], s[%d]);\n", depth - 1, depth - 1, depth);
            break;
        case OP_POKEOFFSET_SETN:
            require_depth(depth, 3);
            emit("*offset_cell(s[%d], s[%d]) = s[%d];\n", depth - 2, depth - 1, depth - 3);
            depth -= 3;
            break;
        case OP_POKE:
            require_depth(depth, 2);
            emit("*cell(s[%d]) = s[%d];\n", depth - 1, depth - 2);
            depth -= 2;
            break;
        case OP_POKE_INC:
            require_depth(depth, 1);
            emit("*cell(s[%d]) += 1.0;\n", --depth);
            break;
        case OP_RAND:
            emit("s[%d] = (double)audio_rand() / RAND_MAX;\n", depth++);
            break;
        case _OP_DSP_PHASOR:
        case _OP_DSP_TRI:
        case _OP_DSP_SAWTOOTH:
        case _OP_DSP_FM:
        case _OP_DSP_SINELOOP:
        case _OP_DSP_DELAY:
        case _OP_DSP_PORT:
        case _OP_DSP_REVERB: {
            const char *kernel = "dclang_phase_step";
            int operands = 3;
            int results = 1;
            switch (instruction.opcode) {
            case _OP_DSP_TRI: kernel = "dclang_tri_sample"; break;
            case _OP_DSP_SAWTOOTH: kernel = "dclang_sawtooth_sample"; break;
            case _OP_DSP_FM: kernel = "dclang_fm_sample"; operands = 5; break;
            case _OP_DSP_SINELOOP: kernel = "dclang_sineloop_sample"; operands = 5; break;
            case _OP_DSP_DELAY: kernel = "dclang_delay_sample"; operands = 2; break;
            case _OP_DSP_PORT: kernel = "dclang_port_sample"; operands = 4; break;
            case _OP_DSP_REVERB: kernel = "dclang_reverb_sample"; operands = 10; results = 2; break;
            default: break;
            }
            require_depth(depth, operands);
            int base = depth - operands;
            emit("if (!%s(memory, sizeof memory / sizeof *memory", kernel);
            for (int i = base; i < depth; ++i)
                emit(", s[%d]", i);
            for (int i = 0; i < results; ++i)
                emit(", &s[%d]", base + i);
            emit(")) abort();\n");
            depth = base + results;
            break;
        }
        case OP_ADD: binary = "+"; break;
        case OP_SUB: binary = "-"; break;
        case OP_MUL: binary = "*"; break;
        case OP_DIV: binary = "/"; break;
        case OP_MOD:
            require_depth(depth, 2);
            --depth;
            emit("s[%d] = audio_mod(s[%d], s[%d]);\n", depth - 1, depth - 1, depth);
            break;
        case OP_EQ: binary = "=="; break;
        case OP_NOTEQ: binary = "!="; break;
        case OP_LT: binary = "<"; break;
        case OP_GT: binary = ">"; break;
        case OP_LTE: binary = "<="; break;
        case OP_GTE: binary = ">="; break;
        case OP_SINE: unary = "sin"; break;
        case OP_COS: unary = "cos"; break;
        case OP_TANH: unary = "tanh"; break;
        case OP_ABS: unary = "fabs"; break;
        case OP_EXP: unary = "exp"; break;
        case OP_ROUND: unary = "(long)round"; break;
        case OP_FLOOR: unary = "(long)floor"; break;
        case OP_CEIL: unary = "(long)ceil"; break;
        default:
            for (const struct primitive *primitive = primitives; primitive->name; ++primitive)
                if (primitive->opcode == instruction.opcode)
                    fail(primitive->name);
            fail("unsupported control flow or opcode");
        }
        if (unary) {
            require_depth(depth, 1);
            emit("s[%d] = %s(s[%d]);\n", depth - 1, unary, depth - 1);
        }
        if (binary) {
            require_depth(depth, 2);
            --depth;
            emit("s[%d] = (s[%d] %s s[%d])%s;\n", depth - 1, depth - 1, binary, depth,
                 instruction.opcode >= OP_EQ && instruction.opcode <= OP_GTE ? " * -1" : "");
        }
        if (depth > DATA_STACK_SIZE)
            fail("audio word exceeds stack capacity");
    }
    return depth;
}

static int emit_word(DCLANG_PTR start, int depth, unsigned int nesting)
{
    DCLANG_PTR end = iptr;
    const char *name = NULL;
    for (DCLANG_PTR i = 0; i < num_user_words; ++i) {
        DCLANG_PTR address = user_words[i].word_start;
        if (address == start)
            name = user_words[i].name;
        else if (address > start && address < end)
            end = address;
    }
    DCLANG_PTR finish = start;
    while (finish < end && prog[finish].opcode != OP_RETURN)
        ++finish;
    if (!name || nesting >= RETURN_STACK_SIZE || finish == end) {
        fprintf(stderr, "dclang2c: cannot inline %s at %zu through %zu (program end %zu)\n",
                name ? name : "<unknown>", (size_t)start, (size_t)end, (size_t)iptr);
        fail("invalid call target or unsupported word boundary");
    }
    unsigned int scope = ++next_scope;
    emit("{\n");
    for (unsigned int i = 0; i < NUMLOCALS; ++i)
        emit("double local%u_%u = 0;\n", scope, i);
    depth = emit_range(start, finish, depth, scope, nesting, name);
    emit("}\n");
    return depth;
}

static void generate(DCLANG_PTR entry, const char *name)
{
    size_t cells = vars_idx > 0 ? vars_idx : 1;
    if (vars_idx && !isfinite(vars[0]))
        fail("non-finite initial cell");
    emit("#define _DEFAULT_SOURCE\n#include <math.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <stdlib.h>\n#include <string.h>\n#include \"dclang_dsp.h\"\n"
         "#pragma STDC FP_CONTRACT OFF\n"
         "static double memory[%zu];\nstatic const double initial[%zu] = {\n"
         "[0] = %a,\n", cells, cells, vars_idx ? vars[0] : 0.0);
    for (size_t i = 1; i < vars_idx; ++i) {
        if (!isfinite(vars[i])) fail("non-finite initial cell");
        if (vars[i] != 0.0 || signbit(vars[i]))
            emit("[%zu] = %a,\n", i, vars[i]);
    }
    emit("};\n"
         "static inline double *cell(double index) {\n"
         "    if (!(index >= 0.0 && index < %zu.0)) abort();\n"
         "    return memory + (size_t)index;\n"
         "}\n"
         "static inline double *offset_cell(double base, double offset) {\n"
         "    const size_t cells = sizeof memory / sizeof *memory;\n"
         "    if (!(base >= 0.0 && base < cells && offset >= 0.0 && offset < cells)) abort();\n"
         "    size_t address = (size_t)base + (size_t)offset;\n"
         "    if (address >= cells) abort();\n"
         "    return memory + address;\n"
         "}\n"
         "#if defined(__GLIBC__)\n"
         "static struct random_data audio_rng;\n"
         "static char audio_rng_state[128];\n"
         "static int audio_rand(void) {\n"
         "    int32_t value;\n"
         "    random_r(&audio_rng, &value);\n"
         "    return value;\n"
         "}\n"
         "#else\n"
         "static int audio_rand(void) { return rand(); }\n"
         "#endif\n"
         "static inline double audio_mod(double x, double divisor) {\n"
         "    if (divisor == 1.0) return dclang_unit_wrap(x);\n"
         "    if (fabs(x) < fabs(divisor)) return x;\n"
         "    return fmod(x, divisor);\n"
         "}\n"
         "void dclang2c_init(void) {\n"
         "    memcpy(memory, initial, sizeof memory);\n"
         "#if defined(__GLIBC__)\n"
         "    memset(&audio_rng, 0, sizeof audio_rng);\n"
         "    initstate_r(%u, audio_rng_state, sizeof audio_rng_state, &audio_rng);\n"
         "#else\n"
         "    srand(%u);\n"
         "#endif\n"
         "    for (unsigned long i = 0; i < %luUL; ++i) (void)audio_rand();\n"
         "}\n"
         "void dclang2c_render(double *output, size_t frames) {\n"
         "    for (size_t frame = 0; frame < frames; ++frame) {\n"
         "        double s[%d];\n", cells, seed, seed, setup_draws, DATA_STACK_SIZE);
    current_word = name;
    current_pc = entry;
    if (emit_word(entry, 0, 0) != 2)
        fail("audio entry must leave exactly two samples");
    emit("        output[2 * frame] = s[0];\n"
         "        output[2 * frame + 1] = s[1];\n"
         "    }\n"
         "}\n");
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: dclang2c source.dc render-word output.c\n");
        return EXIT_FAILURE;
    }
    if (!getenv("DCLANG_LIBS"))
        setenv("DCLANG_LIBS", "./lib", 0);
    dclang_initialize();
    srand(seed);
    if (dclang_import(argv[1]) != 0 || syntax_errors)
        return EXIT_FAILURE;
    DCLANG_LONG entry = dclang_findword(argv[2]);
    if (entry < 0 || def_mode || data_stack_ptr || save_data_stack_ptr) {
        fprintf(stderr, "dclang2c: missing entry word or unbalanced setup\n");
        return EXIT_FAILURE;
    }
    output = tmpfile();
    if (!output) { perror("tmpfile"); return EXIT_FAILURE; }
    generate((DCLANG_PTR)entry, argv[2]);
    if (ferror(output) || fseek(output, 0, SEEK_SET)) {
        perror("generated C");
        return EXIT_FAILURE;
    }
    FILE *destination = fopen(argv[3], "w");
    if (!destination) { perror(argv[3]); return EXIT_FAILURE; }
    int byte;
    while ((byte = fgetc(output)) != EOF && fputc(byte, destination) != EOF)
        ;
    int failed = ferror(output) || ferror(destination);
    failed |= fclose(destination) != 0;
    fclose(output);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
