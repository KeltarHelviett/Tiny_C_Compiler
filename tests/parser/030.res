└── Unit
    └── FuncDef
        ├── Declarator
        │   ├── main
        │   └── Function returning
        │       ├── INT32
        │       └── Params
        │           └── #0
        │               └── VOID
        └── {}
            └── BlockItemList
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── a
                │       │   └── INT32
                │       ├── InitDeclarator
                │       │   ├── b
                │       │   └── INT32
                │       ├── InitDeclarator
                │       │   ├── c
                │       │   └── INT32
                │       ├── InitDeclarator
                │       │   ├── d
                │       │   └── INT32
                │       ├── InitDeclarator
                │       │   ├── e
                │       │   └── INT32
                │       ├── InitDeclarator
                │       │   ├── f
                │       │   └── INT32
                │       └── InitDeclarator
                │           ├── g
                │           └── Array of
                │               ├── INT32
                │               └── 5
                ├── Decl
                │   └── InitDeclaratorList
                │       └── InitDeclarator
                │           ├── max
                │           ├── INT32
                │           └── ?:
                │               ├── >
                │               │   ├── c
                │               │   └── d
                │               ├── c
                │               └── d
                ├── ;
                │   └── ?:
                │       ├── |
                │       │   ├── <=
                │       │   │   ├── +
                │       │   │   │   ├── 5
                │       │   │   │   └── *
                │       │   │   │       ├── 12
                │       │   │   │       └── 8
                │       │   │   └── 5
                │       │   └── >
                │       │       ├── <<
                │       │       │   ├── 9
                │       │       │   └── +
                │       │       │       ├── 3
                │       │       │       └── 1
                │       │       └── 4
                │       ├── &&
                │       │   ├── &
                │       │   │   ├── <<
                │       │   │   │   ├── >>
                │       │   │   │   │   ├── -
                │       │   │   │   │   │   ├── ^
                │       │   │   │   │   │   │   ├── 123
                │       │   │   │   │   │   │   └── 7
                │       │   │   │   │   │   └── d
                │       │   │   │   │   └── 8
                │       │   │   │   └── 7
                │       │   │   └── ==
                │       │   │       ├── +
                │       │   │       │   ├── 12
                │       │   │       │   └── ~
                │       │   │       │       └── 1
                │       │   │       └── <=
                │       │   │           ├── 9
                │       │   │           └── 5
                │       │   └── -
                │       │       ├── +
                │       │       │   ├── f
                │       │       │   └── ++'
                │       │       │       └── []
                │       │       │           ├── g
                │       │       │           └── 5
                │       │       └── 15
                │       └── 12
                └── return
                    └── 0

