/* Host-side compiler for bounded stereo DSP words. See README.md. */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>

static unsigned long setup_draws;
static int syntax_errors;
static size_t target_cell_bytes = sizeof(uintptr_t);

static int compiler_rand(void)
{
    ++setup_draws;
    return rand();
}

#define rand compiler_rand
#define DCLANG_SYNTAX_ERROR() (++syntax_errors)
#define DCLANG_REVERB_CELL_COUNT \
    ((sizeof(dclang_phiverb) + target_cell_bytes - 1) / target_cell_bytes)
#include "libdclang.c"
#undef DCLANG_REVERB_CELL_COUNT
#undef DCLANG_SYNTAX_ERROR
#undef rand
#include <stdarg.h>

static FILE *output;
static unsigned int next_scope;
static const unsigned int seed = 12345;
static const char *current_word;
static DCLANG_PTR current_pc;
static void emit(const char *format, ...);

/* Numeric literals arrive from the host interpreter as untagged cells. Small
 * integers occupy subnormal/zero float bit patterns, while source floats are
 * normal finite values. Emit floats numerically so generated C repacks them
 * for its target cell width; preserve all other cells as integer bits. */
static int cell_is_source_float(DCLANG_CELL cell)
{
    DCLANG_FLT value = dclang_cell_to_float(cell);
    return cell != 0 && isfinite(value) && fpclassify(value) != FP_SUBNORMAL;
}

static void emit_cell(DCLANG_CELL cell)
{
    if (cell_is_source_float(cell))
        emit("dclang_float_to_cell((DCLANG_FLT)%a)",
             (double)dclang_cell_to_float(cell));
    else
        emit("(DCLANG_CELL)UINT64_C(0x%016" PRIxPTR ")", cell);
}

static void emit(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
}

static void fail(const char *message)
{
    fprintf(stderr, "dclang2c-dsp: %s at instruction %zu: %s\n",
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
        int target;
        switch (instruction.opcode) {
        case OP_PUSH:
        case OP_PUSH_NO_CHECK:
            emit("s[%d] = ", depth++);
            emit_cell(instruction.param);
            emit(";\n");
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
            emit("if (s[%d] != 0) {\n", --depth);
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
            emit("{ DCLANG_CELL a = s[%d]; s[%d] = s[%d]; s[%d] = a; }\n",
                 depth - 2, depth - 2, depth - 1, depth - 1);
            break;
        case OP_INC:
            require_depth(depth, 1);
            emit("s[%d] += 1;\n", depth - 1);
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
            emit("*cell(s[%d]) += 1;\n", --depth);
            break;
        case OP_RAND:
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)audio_rand() / (DCLANG_FLT)RAND_MAX);\n", depth++);
            break;
        case _OP_DSP_PHASOR:
        case _OP_DSP_TRI:
        case _OP_DSP_SAWTOOTH:
        case _OP_DSP_WAVETABLE:
        case _OP_DSP_FM:
        case _OP_DSP_SINELOOP:
        case _OP_DSP_DELAY:
        case _OP_DSP_PORT:
        case _OP_DSP_REVERB: {
            const char *kernel = "dclang_phasor_sample";
            int operands = 4;
            int results = 1;
            switch (instruction.opcode) {
            case _OP_DSP_TRI: kernel = "dclang_tri_sample"; break;
            case _OP_DSP_SAWTOOTH: kernel = "dclang_sawtooth_sample"; break;
            case _OP_DSP_WAVETABLE: kernel = "dclang_wavetable_sample"; operands = 7; break;
            case _OP_DSP_FM: kernel = "dclang_fm_sample"; operands = 8; break;
            case _OP_DSP_SINELOOP: kernel = "dclang_sineloop_sample"; operands = 8; break;
            case _OP_DSP_DELAY: kernel = "dclang_delay_sample"; operands = 2; break;
            case _OP_DSP_PORT: kernel = "dclang_port_sample"; operands = 4; break;
            case _OP_DSP_REVERB: kernel = "dclang_reverb_sample"; operands = 6; results = 2; break;
            default: break;
            }
            require_depth(depth, operands);
            int base = depth - operands;
            emit("{ double out0, out1; if (!%s(memory, sizeof memory / sizeof *memory", kernel);
            switch (instruction.opcode) {
            case _OP_DSP_PHASOR:
            case _OP_DSP_TRI:
            case _OP_DSP_SAWTOOTH:
                emit(", dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), (double)s[%d], (double)s[%d]",
                     base, base + 1, base + 2, base + 3);
                break;
            case _OP_DSP_WAVETABLE:
                emit(", dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), (double)s[%d], (double)s[%d], (double)s[%d], (double)s[%d]",
                     base, base + 1, base + 2, base + 3, base + 4, base + 5, base + 6);
                break;
            case _OP_DSP_FM:
                emit(", dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), (double)s[%d], (double)s[%d], (double)s[%d], (double)s[%d]",
                     base, base + 1, base + 2, base + 3, base + 4, base + 5, base + 6, base + 7);
                break;
            case _OP_DSP_SINELOOP:
                emit(", (double)s[%d], dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), (double)s[%d], (double)s[%d], (double)s[%d], (double)s[%d]",
                     base, base + 1, base + 2, base + 3, base + 4, base + 5, base + 6, base + 7);
                break;
            case _OP_DSP_DELAY:
                emit(", (double)s[%d], dclang_cell_to_float(s[%d])", base, base + 1);
                break;
            case _OP_DSP_PORT:
                emit(", dclang_cell_to_float(s[%d]), (double)s[%d], dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d])",
                     base, base + 1, base + 2, base + 3);
                break;
            case _OP_DSP_REVERB:
                emit(", dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), (double)s[%d], dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d])",
                     base, base + 1, base + 2, base + 3, base + 4, base + 5);
                break;
            default: break;
            }
            emit(", &out0");
            if (results == 2) emit(", &out1");
            emit(")) abort(); s[%d] = dclang_float_to_cell((DCLANG_FLT)out0);", base);
            if (results == 2)
                emit(" s[%d] = dclang_float_to_cell((DCLANG_FLT)out1);", base + 1);
            emit(" }\n");
            depth = base + results;
            break;
        }
        case OP_ADD:
        case OP_SUB:
        case OP_MUL: {
            require_depth(depth, 2); --depth;
            const char *op = instruction.opcode == OP_ADD ? "+" : instruction.opcode == OP_SUB ? "-" : "*";
            emit("s[%d] = s[%d] %s s[%d];\n", depth - 1, depth - 1, op, depth);
            break;
        }
        case OP_DIV:
        case OP_MOD: {
            require_depth(depth, 2); --depth;
            const char *op = instruction.opcode == OP_DIV ? "/" : "%";
            emit("s[%d] = (DCLANG_CELL)((intptr_t)s[%d] %s (intptr_t)s[%d]);\n",
                 depth - 1, depth - 1, op, depth);
            break;
        }
        case OP_FADD:
        case OP_FSUB:
        case OP_FMUL:
        case OP_FDIV: {
            require_depth(depth, 2); --depth;
            const char *op = instruction.opcode == OP_FADD ? "+" : instruction.opcode == OP_FSUB ? "-" : instruction.opcode == OP_FMUL ? "*" : "/";
            emit("s[%d] = dclang_float_to_cell(dclang_cell_to_float(s[%d]) %s dclang_cell_to_float(s[%d]));\n",
                 depth - 1, depth - 1, op, depth);
            break;
        }
        case OP_POWER:
            require_depth(depth, 2);
            --depth;
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)DCLANG_POW(dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d])));\n", depth - 1, depth - 1, depth);
            break;
        case OP_FMOD:
            require_depth(depth, 2);
            --depth;
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)audio_mod(dclang_cell_to_float(s[%d]), dclang_cell_to_float(s[%d])));\n", depth - 1, depth - 1, depth);
            break;
        case OP_EQ: case OP_NOTEQ: case OP_LT: case OP_GT: case OP_LTE: case OP_GTE:
        case OP_FEQ: case OP_FNOTEQ: case OP_FLT: case OP_FGT: case OP_FLTE: case OP_FGTE: {
            require_depth(depth, 2); --depth;
            const char *op = instruction.opcode == OP_EQ || instruction.opcode == OP_FEQ ? "=="
                           : instruction.opcode == OP_NOTEQ || instruction.opcode == OP_FNOTEQ ? "!="
                           : instruction.opcode == OP_LT || instruction.opcode == OP_FLT ? "<"
                           : instruction.opcode == OP_GT || instruction.opcode == OP_FGT ? ">"
                           : instruction.opcode == OP_LTE || instruction.opcode == OP_FLTE ? "<=" : ">=";
            int floating = instruction.opcode >= OP_FEQ && instruction.opcode <= OP_FGTE;
            if (floating)
                emit("s[%d] = dclang_cell_to_float(s[%d]) %s dclang_cell_to_float(s[%d]) ? UINTPTR_MAX : 0;\n", depth - 1, depth - 1, op, depth);
            else
                emit("s[%d] = (intptr_t)s[%d] %s (intptr_t)s[%d] ? UINTPTR_MAX : 0;\n", depth - 1, depth - 1, op, depth);
            break;
        }
        case OP_AND:
        case OP_OR:
        case OP_XOR: {
            require_depth(depth, 2);
            const char *bitop = instruction.opcode == OP_AND ? "&"
                              : instruction.opcode == OP_OR ? "|" : "^";
            --depth;
            emit("s[%d] = s[%d] %s s[%d];\n",
                 depth - 1, depth - 1, bitop, depth);
            break;
        }
        case OP_LSHIFT:
        case OP_RSHIFT: {
            require_depth(depth, 2);
            const char *shift = instruction.opcode == OP_LSHIFT ? "<<" : ">>";
            --depth;
            emit("s[%d] = s[%d] %s (s[%d] & (sizeof(DCLANG_CELL) * 8 - 1));\n",
                 depth - 1, depth - 1, shift, depth);
            break;
        }
        case OP_SINE: case OP_COS: case OP_TANH: case OP_EXP:
        case OP_SQRT: case OP_LOG: case OP_LOG2: case OP_LOG10:
        case _OP_DSP_FAST_SIN: case _OP_DSP_FAST_TANH: {
            require_depth(depth, 1);
            const char *fn = instruction.opcode == OP_SINE ? "DCLANG_SIN" : instruction.opcode == OP_COS ? "DCLANG_COS"
                           : instruction.opcode == OP_TANH ? "DCLANG_TANH" : instruction.opcode == OP_EXP ? "DCLANG_EXP"
                           : instruction.opcode == OP_SQRT ? "DCLANG_SQRT" : instruction.opcode == OP_LOG ? "DCLANG_LOG"
                           : instruction.opcode == OP_LOG2 ? "DCLANG_LOG2" : instruction.opcode == OP_LOG10 ? "DCLANG_LOG10"
                           : instruction.opcode == _OP_DSP_FAST_SIN ? "dclang_fast_sin" : "dclang_fast_tanh";
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)%s(dclang_cell_to_float(s[%d])));\n", depth - 1, fn, depth - 1);
            break;
        }
        case OP_ABS:
            require_depth(depth, 1);
            emit("s[%d] = (DCLANG_CELL)((intptr_t)s[%d] < 0 ? -(intptr_t)s[%d] : (intptr_t)s[%d]);\n", depth - 1, depth - 1, depth - 1, depth - 1);
            break;
        case OP_FABS:
            require_depth(depth, 1);
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)DCLANG_FABS(dclang_cell_to_float(s[%d])));\n", depth - 1, depth - 1);
            break;
        case OP_ROUND: case OP_FLOOR: case OP_CEIL:
            require_depth(depth, 1);
            emit("s[%d] = (DCLANG_CELL)(intptr_t)%s(dclang_cell_to_float(s[%d]));\n", depth - 1,
                 instruction.opcode == OP_ROUND ? "round" : instruction.opcode == OP_FLOOR ? "floor" : "ceil", depth - 1);
            break;
        case OP_ITOF:
            require_depth(depth, 1);
            emit("s[%d] = dclang_float_to_cell((DCLANG_FLT)(intptr_t)s[%d]);\n", depth - 1, depth - 1);
            break;
        case OP_FTOI:
            require_depth(depth, 1);
            emit("s[%d] = (DCLANG_CELL)(intptr_t)dclang_cell_to_float(s[%d]);\n", depth - 1, depth - 1);
            break;
        case OP_NOT:
            require_depth(depth, 1); emit("s[%d] = ~s[%d];\n", depth - 1, depth - 1); break;
        default:
            for (const struct primitive *primitive = primitives; primitive->name; ++primitive)
                if (primitive->opcode == instruction.opcode)
                    fail(primitive->name);
            fail("unsupported control flow or opcode");
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
        fprintf(stderr, "dclang2c-dsp: cannot inline %s at %zu through %zu (program end %zu)\n",
                name ? name : "<unknown>", (size_t)start, (size_t)end, (size_t)iptr);
        fail("invalid call target or unsupported word boundary");
    }
    unsigned int scope = ++next_scope;
    emit("{\n");
    for (unsigned int i = 0; i < NUMLOCALS; ++i)
        emit("DCLANG_CELL local%u_%u = 0;\n", scope, i);
    depth = emit_range(start, finish, depth, scope, nesting, name);
    emit("}\n");
    return depth;
}

static void generate(DCLANG_PTR entry, const char *name)
{
    size_t cells = vars_idx > 0 ? vars_idx : 1;
    emit("#define _DEFAULT_SOURCE\n#include <inttypes.h>\n#include <math.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <stdlib.h>\n#include <string.h>\n"
         "#if UINTPTR_MAX != %s\n#error generated dclang2c-dsp cell width does not match this target\n#endif\n",
         target_cell_bytes == 4 ? "UINT32_MAX" : "UINT64_MAX");
    emit(
         "typedef uintptr_t DCLANG_CELL;\n"
         "#if UINTPTR_MAX == UINT64_MAX\ntypedef double DCLANG_FLT;\n#elif UINTPTR_MAX == UINT32_MAX\ntypedef float DCLANG_FLT;\n#else\n#error unsupported pointer width\n#endif\n"
         "#if defined(DCLANG_MATH_NEON)\n"
         "#include <libraries/math_neon/math_neon.h>\n"
         "#define DCLANG_SIN sinf_neon\n#define DCLANG_COS cosf_neon\n#define DCLANG_TANH tanhf_neon\n#define DCLANG_EXP expf_neon\n#define DCLANG_LOG logf_neon\n#define DCLANG_LOG2 log2f\n#define DCLANG_LOG10 log10f_neon\n#define DCLANG_POW powf_neon\n#define DCLANG_SQRT sqrtf_neon\n#define DCLANG_FABS fabsf_neon\n"
         "#elif UINTPTR_MAX == UINT32_MAX\n"
         "#define DCLANG_SIN sinf\n#define DCLANG_COS cosf\n#define DCLANG_TANH tanhf\n#define DCLANG_EXP expf\n#define DCLANG_LOG logf\n#define DCLANG_LOG2 log2f\n#define DCLANG_LOG10 log10f\n#define DCLANG_POW powf\n#define DCLANG_SQRT sqrtf\n#define DCLANG_FABS fabsf\n"
         "#else\n"
         "#define DCLANG_SIN sin\n#define DCLANG_COS cos\n#define DCLANG_TANH tanh\n#define DCLANG_EXP exp\n#define DCLANG_LOG log\n#define DCLANG_LOG2 log2\n#define DCLANG_LOG10 log10\n#define DCLANG_POW pow\n#define DCLANG_SQRT sqrt\n#define DCLANG_FABS fabs\n"
         "#endif\n"
         "static inline DCLANG_CELL dclang_float_to_cell(DCLANG_FLT value) { DCLANG_CELL cell; memcpy(&cell, &value, sizeof cell); return cell; }\n"
         "static inline DCLANG_FLT dclang_cell_to_float(DCLANG_CELL cell) { DCLANG_FLT value; memcpy(&value, &cell, sizeof value); return value; }\n"
         "#include \"dclang_dsp.h\"\n"
         "#pragma STDC FP_CONTRACT OFF\n"
         "#ifndef DCLANG2C_SAMPLE_TYPE\n#define DCLANG2C_SAMPLE_TYPE float\n#endif\n"
         "typedef DCLANG2C_SAMPLE_TYPE dclang2c_sample;\n"
         "static DCLANG_CELL memory[%zu];\nstatic const DCLANG_CELL initial[%zu] = {\n"
         "[0] = ", cells, cells);
    if (vars_idx && !cell_is_source_float(vars[0]))
        emit_cell(vars[0]);
    else
        emit("0");
    emit(",\n");
    for (size_t i = 1; i < vars_idx; ++i) {
        if (vars[i] != 0 && !cell_is_source_float(vars[i])) {
            emit("[%zu] = ", i);
            emit_cell(vars[i]);
            emit(",\n");
        }
    }
    emit("};\n"
         "static inline DCLANG_CELL *cell(DCLANG_CELL index) {\n"
         "    if (index >= %zu) abort();\n"
         "    return memory + index;\n"
         "}\n"
         "static inline DCLANG_CELL *offset_cell(DCLANG_CELL base, DCLANG_CELL offset) {\n"
         "    const size_t cells = sizeof memory / sizeof *memory;\n"
         "    if (base >= cells || offset >= cells) abort();\n"
         "    size_t address = base + offset;\n"
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
         "    memcpy(memory, initial, sizeof memory);\n", cells);
    for (size_t i = 0; i < vars_idx; ++i) {
        if (cell_is_source_float(vars[i])) {
            emit("    memory[%zu] = ", i);
            emit_cell(vars[i]);
            emit(";\n");
        }
    }
    emit(
         "#if defined(__GLIBC__)\n"
         "    memset(&audio_rng, 0, sizeof audio_rng);\n"
         "    initstate_r(%u, audio_rng_state, sizeof audio_rng_state, &audio_rng);\n"
         "#else\n"
         "    srand(%u);\n"
         "#endif\n"
         "    for (unsigned long i = 0; i < %luUL; ++i) (void)audio_rand();\n"
         "}\n"
         "int dclang2c_control_index(const char *name) {\n",
         seed, seed, setup_draws);
    /* Match interpreter lookup: the most recent binding wins. */
    for (DCLANG_LONG i = (DCLANG_LONG)var_map_idx - 1; i >= 0; --i)
        emit("    if (strcmp(name, \"%s\") == 0) return %zu;\n",
             var_keys[i], (size_t)var_vals[i]);
    emit(
         "    return -1;\n"
         "}\n"
         "uint32_t dclang2c_get_u32(unsigned int index) {\n"
         "    return index < %zu ? (uint32_t)memory[index] : 0;\n"
         "}\n"
         "DCLANG_FLT dclang2c_get_float(unsigned int index) {\n"
         "    return index < %zu ? dclang_cell_to_float(memory[index]) : (DCLANG_FLT)0;\n"
         "}\n"
         "void dclang2c_set_u32(unsigned int index, uint32_t value) {\n"
         "    if (index < %zu) memory[index] = (DCLANG_CELL)value;\n"
         "}\n"
         "void dclang2c_set_float(unsigned int index, DCLANG_FLT value) {\n"
         "    if (index < %zu) memory[index] = dclang_float_to_cell(value);\n"
         "}\n"
         "void dclang2c_render(dclang2c_sample *output, size_t frames) {\n"
         "    for (size_t frame = 0; frame < frames; ++frame) {\n"
         "        DCLANG_CELL s[%d];\n", cells, cells, cells, cells, DATA_STACK_SIZE);
    current_word = name;
    current_pc = entry;
    if (emit_word(entry, 0, 0) != 2)
        fail("audio entry must leave exactly two samples");
    emit("        output[2 * frame] = (dclang2c_sample)dclang_cell_to_float(s[0]);\n"
         "        output[2 * frame + 1] = (dclang2c_sample)dclang_cell_to_float(s[1]);\n"
         "    }\n"
         "}\n");
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: dclang2c-dsp source.dc render-word output.c\n");
        return EXIT_FAILURE;
    }
    if (!getenv("DCLANG_LIBS"))
        setenv("DCLANG_LIBS", "./lib", 0);
    const char *cell_bits = getenv("DCLANG2C_CELL_BITS");
    if (cell_bits) {
        if (strcmp(cell_bits, "32") == 0)
            target_cell_bytes = 4;
        else if (strcmp(cell_bits, "64") == 0)
            target_cell_bytes = 8;
        else {
            fprintf(stderr, "dclang2c-dsp: DCLANG2C_CELL_BITS must be 32 or 64\n");
            return EXIT_FAILURE;
        }
    }
    dclang_initialize();
    srand(seed);
    if (dclang_import(argv[1]) != 0 || syntax_errors)
        return EXIT_FAILURE;
    DCLANG_LONG entry = dclang_findword(argv[2]);
    if (entry < 0 || def_mode || data_stack_ptr || save_data_stack_ptr) {
        fprintf(stderr, "dclang2c-dsp: missing entry word or unbalanced setup\n");
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
