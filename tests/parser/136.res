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
                │       └── InitDeclarator
                │           ├── a
                │           └── Qualified 0
                │               └── INT32
                ├── Decl
                │   └── InitDeclaratorList
                │       └── InitDeclarator
                │           ├── b
                │           └── Qualified 0
                │               └── UINT32
                ├── ;
                │   └── -
                │       └── a
                ├── ;
                │   └── +
                │       └── a
                ├── ;
                │   └── -
                │       └── typcast
                │           ├── INT32
                │           └── b
                └── ;
                    └── +
                        └── b

