"""
Provides some cpp functions
"""
# import sys
# <OutDir>$(SolutionDir)$(Platform)\$(Configuration)\</OutDir>
# VS_OUTPUT_PATH = r"mixed-cpp-python\x64\Release"
# VS_OUTPUT_PATH = r"mixed-cpp-python\x64\Debug"
# sys.path.append(VS_OUTPUT_PATH)
import spam

def main():
    print(spam)
    print("Module path:", getattr(spam, '__file__', 'Unknown (Built-in/Namespace)'))
    print("Available names:", dir(spam))

    print(spam.add(1,2))
    print(spam.inc(123))
    print(spam.say_hello("Alex"))

if __name__ == '__main__':
    main()
