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
                │           └── Pointer to
                │               └── struct A
                └── Decl
                    └── InitDeclaratorList
                        └── InitDeclarator
                            ├── c
                            ├── Qualified 0
                            │   └── FLOAT
                            └── Initializer 
                                └── typcast
                                    ├── Qualified 0
                                    │   └── FLOAT
                                    └── ?:
                                        ├── ==
                                        │   ├── ->
                                        │   │   ├── a
                                        │   │   └── a
                                        │   └── 123
                                        ├── 1
                                        └── 2

