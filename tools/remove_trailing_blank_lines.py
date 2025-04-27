import sys


def remove_trailing_blank_lines(file_path: str):
    with open(file_path, "r") as f:
        content = f.read()

    # Normalize line endings to LF
    lines = content.replace("\r\n", "\n").split("\n")

    # remove blank lines
    while lines and lines[-1].strip() == "":
        lines.pop()

    if len(lines) == 0:
        # end empty file with a new line
        modified_content = "\n"
    else:
        # Join lines with LF and ensure the last character is a newline
        modified_content = "\n".join(lines) + "\n"

    if modified_content != content:
        with open(file_path, "w") as f:
            f.write(modified_content)


if __name__ == "__main__":
    for file_path in sys.argv[1:]:
        remove_trailing_blank_lines(file_path)
