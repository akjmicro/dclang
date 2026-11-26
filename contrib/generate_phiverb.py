#!/usr/bin/env python3
"""Generate dclang_phiverb.h from Aaron Krister Johnson's phiverb.lib."""

import argparse
import subprocess
import tempfile
from pathlib import Path


REMOVED_FUNCTIONS = (
    "newdclang_phiverb",
    "deletedclang_phiverb",
    "metadatadclang_phiverb",
    "getSampleRatedclang_phiverb",
    "getNumInputsdclang_phiverb",
    "getNumOutputsdclang_phiverb",
    "buildUserInterfacedclang_phiverb",
)

STATIC_FUNCTIONS = (
    "classInitdclang_phiverb",
    "instanceResetUserInterfacedclang_phiverb",
    "instanceCleardclang_phiverb",
    "instanceConstantsdclang_phiverb",
    "instanceInitdclang_phiverb",
    "initdclang_phiverb",
    "computedclang_phiverb",
)


def remove_function(source, name):
    marker = name + "("
    name_at = source.index(marker)
    start = source.rfind("\n", 0, name_at) + 1
    brace = source.index("{", name_at)
    depth = 0
    for end in range(brace, len(source)):
        if source[end] == "{":
            depth += 1
        elif source[end] == "}":
            depth -= 1
            if depth == 0:
                while end + 1 < len(source) and source[end + 1] == "\n":
                    end += 1
                return source[:start] + source[end + 1:]
    raise ValueError(f"unterminated generated function: {name}")


def postprocess(source):
    source = source.replace(
        "/* ------------------------------------------------------------",
        "/* Generated from Aaron Krister Johnson's phiverb.lib. The UI and metadata\n"
        " * entry points were removed so this header can serve the interpreter and\n"
        " * standalone dclang2c-dsp output without a Faust architecture dependency.\n"
        " * ------------------------------------------------------------",
        1,
    )
    source = source.replace(
        "typedef struct {",
        "typedef struct\n"
        "#if defined(__GNUC__) || defined(__clang__)\n"
        "    __attribute__((may_alias))\n"
        "#endif\n"
        "{",
        1,
    )
    for name in REMOVED_FUNCTIONS:
        source = remove_function(source, name)
    for name in STATIC_FUNCTIONS:
        source = source.replace(f"void {name}(", f"static void {name}(", 1)
    if "MetaGlue" in source or "UIGlue" in source or "calloc(" in source:
        raise ValueError("Faust architecture entry points remain after post-processing")
    return "\n".join(line.rstrip() for line in source.splitlines()) + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("phiverb_lib", type=Path)
    parser.add_argument("output", nargs="?", type=Path, default=Path("dclang_phiverb.h"))
    args = parser.parse_args()
    library = args.phiverb_lib.resolve()
    if not library.is_file():
        parser.error(f"not a file: {library}")

    wrapper = f'''import("{library}");

feedback = hslider("feedback", 0.7, 0.0, 0.999, 0.001);
cutoff = hslider("cutoff", 0.75, 0.0, 1.0, 0.001);

process = _,_ : phiverb(sqrt(feedback), feedback, 1.0-cutoff);
'''
    with tempfile.TemporaryDirectory(prefix="dclang-phiverb-") as directory:
        dsp = Path(directory) / "dclang-phiverb.dsp"
        raw = Path(directory) / "dclang_phiverb.h"
        dsp.write_text(wrapper)
        subprocess.run(
            ["faust", "-lang", "c", "-single", "-cn", "dclang_phiverb",
             str(dsp), "-o", str(raw)],
            check=True,
        )
        args.output.write_text(postprocess(raw.read_text()))


if __name__ == "__main__":
    main()
