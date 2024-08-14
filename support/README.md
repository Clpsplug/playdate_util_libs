# Playdate Utility Library Support Files

This module acts as a 'meta-module' for other modules in this library,
meaning that it will serve as a batch operator across all the main modules.

## Functions

### pdUtil_InitializeAll(void* pd)

```c
void pdUtil_InitializeAll(void* pd);
```

Initializes all the modules included in this library.

#### Parameters

* [in] `pd` Playdate context object

### pdUtil_FinalizeAll(void)

```c
void pdUtil_FinalizeAll(void);
```

Finalizes all the modules included in this library.

> [!WARNING]  
> Do NOT use this function unless you have used `pdUtil_InitializeAll(pd)`
> to initialize all the modules.  
> This function assumes that all the modules are initialized,
> thus the behavior when any module isn't initialized is undefined.
