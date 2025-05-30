# Simpl Compiler/Interpreter

SIMPL, which stands for **S**imple **I**ntegrated **M**inimalistic **P**rogramming **L**anguage, is a Turing-complete language designed with education in mind. This project provides a compiler and interpreter for Simpl.

The primary goal of this project is to serve as an educational tool for students learning about the fundamental concepts of compiler construction. The codebase is written in C++ and is structured to be as clear and straightforward as possible, allowing students with a basic understanding of C++ to follow along and grasp the core ideas behind each compiler phase. We believe that by examining and modifying this project, students can gain practical insights into how compilers work.

## Architecture Diagram

![image](https://github.com/user-attachments/assets/87c549b0-4a47-48f3-a159-702f466f37f7)


## Components

### Lexer
The **Lexer** (or lexical analyzer/scanner) is the initial stage of the compilation process.
*   **Purpose:** It reads the raw Simpl source code character by character and groups these characters into a stream of meaningful symbols called *tokens*. Tokens are the smallest individual units of a program, like keywords (`if`, `while`), identifiers (variable names), operators (`+`, `=`), literals (numbers like `123`, strings like `"hello"`), and punctuation (parentheses, semicolons).
*   **Design Choices:** The lexer for Simpl is designed to be straightforward. It typically uses regular expressions or finite automata concepts to recognize token patterns. The emphasis is on clarity, making it easy for students to see how raw text is converted into a structured token stream. Error handling at this stage involves identifying invalid characters or malformed tokens.
*   **Contribution:** The lexer transforms the unstructured source text into a format that the parser can understand, effectively abstracting away the character-level details of the source code. This separation of concerns is a fundamental principle in compiler design.

### Parser
The **Parser** (or syntax analyzer) is the second phase.
*   **Purpose:** It takes the flat list of tokens produced by the lexer and attempts to build a hierarchical structure that reflects the grammatical rules of the Simpl language. This structure is most commonly an *Abstract Syntax Tree (AST)*. The AST represents the syntactic structure of the code, showing how tokens are grouped into expressions, statements, and other language constructs.
*   **Design Choices:** For Simpl, a recursive descent parser is often chosen for its simplicity and direct mapping to the language's grammar rules (often expressed in Backus-Naur Form or EBNF). This makes the parsing logic relatively easy to follow. The parser also reports syntax errors if the token stream cannot be structured according to the Simpl grammar (e.g., a missing semicolon or mismatched parentheses).
*   **Contribution:** The parser ensures that the program is syntactically correct and creates the AST, which is a crucial data structure used by subsequent phases for semantic analysis and code generation.

### Semantic Analyzer
The **Semantic Analyzer** is the third phase, operating on the AST produced by the parser.
*   **Purpose:** While the parser checks for correct syntax, the semantic analyzer checks for *meaning* and logical consistency. It enforces rules that are not easily captured by syntax alone. Key tasks include:
    *   **Type Checking:** Ensuring that operations are performed on compatible data types (e.g., you can't add a string to an integer without explicit conversion in many languages).
    *   **Scope Resolution:** Verifying that variables and functions are declared before they are used and are used within their correct scope.
    *   **Argument Matching:** Checking that functions are called with the correct number and types of arguments.
*   **Design Choices:** The semantic analyzer for Simpl traverses the AST, collecting information about identifiers (in a symbol table) and verifying language rules. Type checking rules are kept simple to illustrate the concepts clearly. Error messages are designed to be informative, helping the user understand why their code is semantically incorrect.
*   **Contribution:** This phase catches a wide range of common programming errors and enriches the AST with type information and resolved identifier references, preparing it for translation into a lower-level form.

### IR Generator
The **Intermediate Representation (IR) Generator** is the fourth phase.
*   **Purpose:** After the AST has been successfully parsed and semantically validated, it is translated into an *Intermediate Representation*. The IR is a lower-level, abstract form of the program that is independent of both the source language (Simpl) and the target machine architecture. It's designed to be suitable for optimization and straightforward translation to machine code or direct interpretation.
*   **Design Choices:** A common IR for educational compilers is a three-address code (TAC) or a stack-based representation. For Simpl, the IR is designed to be simple yet expressive enough to represent all language constructs. The generation process involves another traversal of the (potentially annotated) AST.
*   **Contribution:** The IR provides a clean separation between the front-end (lexer, parser, semantic analyzer) and the back-end (optimizer, code generator/interpreter) of the compiler. This modularity makes it easier to retarget the compiler to different machines or to add different front-ends for other languages.

### Interpreter
The **Interpreter** is one of the possible back-ends for the Simpl language.
*   **Purpose:** Instead of generating machine code that is then executed by the CPU, the interpreter directly executes the program instructions as represented in the Intermediate Representation (or sometimes, directly from the AST). It simulates the execution of the Simpl program.
*   **Design Choices:** The Simpl interpreter will typically loop through the IR instructions (or traverse the AST) and perform the actions specified by each instruction. This involves managing a runtime environment, including memory for variables and the call stack for function calls. The focus is on a clear and correct simulation of Simpl's semantics.
*   **Contribution:** The interpreter provides an immediate way to run Simpl programs without a separate compilation-to-machine-code step. This is often simpler to implement than a full code generator and can be very useful for debugging and rapid prototyping. It directly demonstrates the execution flow of Simpl programs based on the structures built by the preceding phases.

## Building and Running the Project

This section explains how to build the Simpl compiler/interpreter from source and how to run it on your Simpl programs.

### Prerequisites
*   A C++ compiler that supports C++11 or later (e.g., GCC, Clang).
*   `make` utility for build automation.

### Building the Project
The project utilizes a `makefile` to streamline the compilation process.

1.  **Navigate to the project root:**
    Open your terminal or command prompt and change the directory to the root folder of the Simpl project where the `makefile` is located.

2.  **Build the executables:**
    To compile all parts of the Simpl project (lexer, parser, interpreter, etc., as defined in the `makefile`), execute the following command:
    ```bash
    make run
    ```

    This command will invoke the C++ compiler to compile the source files and link them, creating the executable files in the project directory (or a specified output directory, like `bin/`, if configured in the `makefile`). Check the `makefile` for specific target names if you want to build individual components (e.g., `make lexer`).

### Running a Simpl Program
Once the project has been successfully built, you will have one or more executables. The exact name of the executable will depend on how the `makefile` is structured (e.g., `simpl_interpreter.exe`, `simplc.exe`, or just `simpl.exe`).

To execute a Simpl program (which are typically saved in files with a `.simpl` extension):

1.  **Ensure the executable is in your PATH or provide the path to it.**
    If the executable is in the current directory, you can usually run it with `./executable_name`.

2.  **Run the command:**
    For example, if your main interpreter/compiler executable is named `simpl_interpreter.exe` and you have a test file named `my_program.simpl`, you would run:
    ```bash
    ./simpl_interpreter.exe my_program.simpl
    ```
    Or, if the `makefile` produces separate executables for each phase for educational purposes (e.g., `simpl_lexer.exe`, `simpl_parser.exe`):
    ```bash
    ./simpl_lexer.exe my_program.simpl  # To see the token stream
    ./simpl_parser.exe my_program.simpl # To check syntax and potentially see an AST representation
    ```
    Always replace `my_program.simpl` with the path to the actual Simpl file you want to process. The output will depend on the specific component being run (e.g., the lexer might print tokens, the interpreter might print program output).

## Testing the Compiler/Interpreter

The Simpl project includes a dedicated `testing/` directory. This directory is crucial for verifying the correctness of the compiler/interpreter and for understanding how various language features are expected to behave.

*   **Contents:** The `testing/` directory contains a collection of Simpl source files (`.simpl` files). These files are designed to cover:
    *   **Valid constructs:** Programs that correctly use all features of the Simpl language.
    *   **Edge cases:** Scenarios that might be tricky to handle.
    *   **Invalid constructs:** Programs that contain deliberate syntax or semantic errors to test the error-reporting capabilities of the compiler/interpreter.

*   **Purpose:**
    *   **Verification:** You can run these test files through your compiled Simpl executables to ensure that each component (lexer, parser, semantic analyzer, interpreter) behaves as expected.
    *   **Learning:** By examining these test files, students can get a clearer picture of Simpl's syntax, semantics, and capabilities.
    *   **Development:** When modifying or extending the compiler, these tests serve as regression checks to ensure existing functionality isn't broken.

*   **How to Use:**
    Navigate to the `testing/` directory or run the executables from the project root, providing the path to the test files. For example:
    ```bash
    ./simpl_interpreter.exe testing/valid_program.simpl
    ./simpl_lexer.exe testing/syntactic_error.simpl
    ```
    Observe the output and compare it against the expected behavior for each test case. Some test cases might also have corresponding `.expected_output` files that you can use for comparison.
