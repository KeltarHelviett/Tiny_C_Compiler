└── Unit
    └── FuncDef
        ├── Declarator
        │   ├── main
        │   └── Function returning
        │       └── Qualified 0
        │           └── INT32
        │       └── Params
        └── {}
            └── BlockItemList
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── a
                │       │   └── Qualified 0
                │       │       └── INT8
                │       └── InitDeclarator
                │           ├── b
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── INT8
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── c
                │       │   └── Qualified 0
                │       │       └── INT32
                │       └── InitDeclarator
                │           ├── d
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── INT32
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── e
                │       │   └── Qualified 0
                │       │       └── INT64
                │       └── InitDeclarator
                │           ├── f
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── INT64
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── g
                │       │   └── Qualified 0
                │       │       └── FLOAT
                │       └── InitDeclarator
                │           ├── h
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── FLOAT
                ├── Decl
                │   └── InitDeclaratorList
                │       ├── InitDeclarator
                │       │   ├── i
                │       │   └── Qualified 0
                │       │       └── DOUBLE
                │       └── InitDeclarator
                │           ├── j
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── DOUBLE
                ├── Decl
                │   └── InitDeclaratorList
                │       └── InitDeclarator
                │           ├── v
                │           └── Pointer to
                │               └── Qualified 0
                │                   └── VOID
                ├── ;
                │   └── >
                │       ├── typcast
                │       │   ├── DOUBLE
                │       │   └── <
                │       │       ├── typcast
                │       │       │   ├── FLOAT
                │       │       │   └── >=
                │       │       │       ├── typcast
                │       │       │       │   ├── INT64
                │       │       │       │   └── <=
                │       │       │       │       ├── typcast
                │       │       │       │       │   ├── INT32
                │       │       │       │       │   └── a
                │       │       │       │       └── c
                │       │       │       └── e
                │       │       └── g
                │       └── i
                ├── ;
                │   └── !=
                │       ├── typcast
                │       │   ├── DOUBLE
                │       │   └── ==
                │       │       ├── typcast
                │       │       │   ├── FLOAT
                │       │       │   └── !=
                │       │       │       ├── typcast
                │       │       │       │   ├── INT64
                │       │       │       │   └── ==
                │       │       │       │       ├── typcast
                │       │       │       │       │   ├── INT32
                │       │       │       │       │   └── a
                │       │       │       │       └── c
                │       │       │       └── e
                │       │       └── g
                │       └── i
                ├── ;
                │   └── ==
                │       ├── b
                │       └── v
                └── ;
                    └── ==
                        ├── b
                        └── b

