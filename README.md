# smplc

A compiler for smpl language - for UCI Advanced compiler course project

# Prerequisites

* CMake

# Build

```shell
mkdir build
cd build
cmake ..
make
```

# Run

```shell
smplc [Options] <input_file>
```

**It still under development and not really generate assembly now. Use the instruction below to generate visualized IR in Graphviz format**

```shell
smplc --visualize_ir <output_file> <input_file>
```

### Options

* `--parser_debug` : Generate verbose parser messages for debug parser
* `--parse_only` : Parse only without generate IR
* `--no_cse` : Not perform Common Subexpression Elimination

# Test

```shell
cd build
ctest
```

# SMPL Syntax

letter = “a” | “b” | ... | “z”

digit = “0” | “1” | ... | “9”

relOp = “==“ | “!=“ | “<“ | “<=“ | “>“ | “>=“

ident = letter {letter | digit}

number = digit {digit}

designator = ident{ "[" expression "]" }

factor = designator | number | “(“ expression “)” | funcCall

term = factor { (“*” | “/”) factor}

expression = term {(“+” | “-”) term}

relation = expression relOp expression

assignment = “let” designator “<-” expression

funcCall = “call” ident [ “(“ [expression { “,” expression } ] “)” ]

ifStatement = “if” relation “then” statSequence [ “else” statSequence ] “fi”

whileStatement = “while” relation “do” StatSequence “od”

returnStatement = “return” [ expression ]

statement = assignment | funcCall | ifStatement | whileStatement | returnStatement

statSequence = statement { “;” statement } [ “;” ]

typeDecl = “var” | “array” “[“ number “]” { “[“ number “]” }

varDecl = typeDecl indent { “,” ident } “;”

funcDecl = [ “void” ] “function” ident formalParam “;” funcBody “;”

formalParam = “(“ [ident { “,” ident }] “)”

funcBody = { varDecl } “{” [ statSequence ] “}”

computation = “main” { varDecl } { funcDecl } “{” statSequence “}” “.”

## Comment & spaces

space = "\v" | "\r" | " " | "\t" | "\n"

singleLineComment = "//" [any-character-than-newline]* "\n"

multiLineComment = "/\*" [any-character]\* "*/"

## Predefined Functions

InputNum() read a number from the standard input
OutputNum(x) write a number to the standard output
OutputNewLine() write a carriage return to the standard output
