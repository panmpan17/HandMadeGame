import os
import argparse

class Terminal:
    LIGHT_BLUE = '\033[96m'
    GREEN = '\033[92m'
    RED = '\033[91m'
    PINK = "\033[38;5;213m"
    YELLOW = '\033[93m'
    BOLD = '\033[1m'
    LIGHT_GRAY = "\033[38;5;250m"
    GREY = '\033[90m'
    BLACK = '\033[30m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

    @classmethod
    def info(cls, *args):
        print(cls.LIGHT_BLUE + " ".join(args) + cls.END)

    @classmethod
    def ok(cls, msg):
        print(cls.GREEN + str(msg) + cls.END)

    @classmethod
    def warning(cls, msg):
        print(cls.YELLOW + str(msg) + cls.END)

    @classmethod
    def fail(cls, *args):
        print(cls.RED + " ".join(args) + cls.END)

    @staticmethod
    def clear(cls):
        os.system('cls' if os.name == 'nt' else 'clear')

    @staticmethod
    def play_sound(sound_name, volume=10):
        os.system("afplay /System/Library/Sounds/%s -v %s" % (sound_name, volume))
    
    @classmethod
    def test_colors(cls):
        print(cls.LIGHT_BLUE + "Light Blue" + cls.END)
        print(cls.GREEN + "Green" + cls.END)
        print(cls.RED + "Red" + cls.END)
        print(cls.PINK + "Pink" + cls.END)
        print(cls.YELLOW + "Yellow" + cls.END)
        print(cls.BOLD + "Bold" + cls.END)
        print(cls.LIGHT_GRAY + "Light Gray" + cls.END)
        print(cls.GREY + "Grey" + cls.END)
        print(cls.BLACK + "Black" + cls.END)
        print(cls.UNDERLINE + "Underline" + cls.END)

    @staticmethod
    def test_all_colors():
        for i in range(0, 256):
            print(f"\033[38;5;{i}mColor {i}\033[0m")


class ColoredArgumentParser(argparse.ArgumentParser):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs, formatter_class=CustomHelpFormatter)

    def error(self, message):
        # Customize the error message with color
        error_message = f"{Terminal.RED}error: {message}{Terminal.END}\n\n"
        # usage_message = f"{Terminal.GREY}Usage: {self.format_usage()}{Terminal.END}"
        self.exit(2, error_message + self.format_help())


class CustomHelpFormatter(argparse.HelpFormatter):
    def add_usage(self, usage, actions, groups, prefix=None):
        # Custom usage message with color
        if prefix is None:
            prefix = Terminal.YELLOW + "Usage: " + Terminal.END
        return super().add_usage(usage, actions, groups, prefix)

    def start_section(self, heading):
        # Custom section heading with color
        heading = Terminal.LIGHT_BLUE + Terminal.UNDERLINE + heading.capitalize() + Terminal.END
        super().start_section(heading)

    def add_argument(self, action):
        # Highlight arguments
        if action.help is not argparse.SUPPRESS:
            if action.help is not None:
                action.help = Terminal.LIGHT_GRAY + action.help + Terminal.END
        super().add_argument(action)


def hex_to_rgb(_hex):
    _hex = _hex.lstrip('#')
    return tuple(int(_hex[i:i+2], 16) for i in (0, 2, 4))

def multiline_input():
    lines = []
    count = 200
    while count > 0:
        try:
            line = input()
        except KeyboardInterrupt:
            exit(1)
        if line == "" or line == "exit":
            break
        lines.append(line)
        count += 1
    
    return lines

if __name__ == '__main__':
    print("Terminal available colors:")
    Terminal.test_colors()

    print("\nAll 256 colors:")
    Terminal.test_all_colors()

    print("\nTest hex")
    print(hex_to_rgb("#2E3A40"))
    print(hex_to_rgb("#265013"))
    print(hex_to_rgb("#041E58"))
    print(hex_to_rgb("#750201"))
    print(hex_to_rgb("#075048"))
    print(hex_to_rgb("#5A29C1"))
