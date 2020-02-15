# nm-otool

Print informations on Mach-O architecture files.
* nm - display symbol table
* otool - object file displaying tool

```rtfm
man nm
man otool
```

Project to learn about Mach-O binaries structure, the `ft_nm` executable replicas `nm`, and `ft_otool` replicates `otool -t`

Even if those commands also exists for ELF binaries on Linux, this project is only made for Mach-O binaries and will not be compatible as structures are different.

This project was made under the 42 coding convention called the 'norme' (which I do not approve for various reasons)

- 42 Coding convention pdf: [en 🇬🇧](https://cdn.intra.42.fr/pdf/pdf/960/norme.en.pdf) / [fr 🇫🇷](https://github.com/Binary-Hackers/42_Subjects/raw/master/04_Norme/norme_2_0_1.pdf)

### Examples

```bash
~/github.com/jjaniec/nm-otool
❯ uname -s
Darwin

~/github.com/jjaniec/nm-otool
❯ file /bin/ls
/bin/ls: Mach-O 64-bit executable x86_64
```

```bash
~/github.com/jjaniec/nm-otool
❯ ./ft_nm /bin/ls
                 U __DefaultRuneLocale
                 U ___assert_rtn
                 U ___bzero
                 U ___error
                 U ___maskrune
                 U ___snprintf_chk
                 U ___stack_chk_fail
                 U ___stack_chk_guard
                 U ___stderrp
                 U ___stdoutp
                 U ___tolower
0000000100000000 T __mh_execute_header
                 U _acl_free
                 U _acl_get_entry
                 U _acl_get_flag_np
                 U _acl_get_flagset_np
                 U _acl_get_link_np
                 U _acl_get_perm_np
                 U _acl_get_permset
                 U _acl_get_qualifier
                 U _acl_get_tag_type
                 U _atoi
                 U _calloc
                 U _compat_mode
                 U _err
                 U _exit
                 U _fflagstostr
                 U _fprintf
                 U _fputs
                 U _free
                 U _fts_children$INODE64
                 U _fts_close$INODE64
                 U _fts_open$INODE64
                 U _fts_read$INODE64
                 U _fts_set$INODE64
                 U _fwrite
                 U _getbsize
                 U _getenv
                 U _getopt
                 U _getpid
                 U _getuid
                 U _getxattr
                 U _group_from_gid
                 U _humanize_number
                 U _ioctl
                 U _isatty
                 U _kill
                 U _listxattr
                 U _localtime
                 U _malloc
                 U _mbr_identifier_translate
                 U _mbrtowc
                 U _memchr
                 U _nl_langinfo
                 U _optind
                 U _printf
                 U _putchar
                 U _readlink
                 U _realloc
                 U _reallocf
                 U _setenv
                 U _setlocale
                 U _signal
                 U _sscanf
                 U _strcoll
                 U _strcpy
                 U _strdup
                 U _strerror
                 U _strftime
                 U _strlen
                 U _strmode
                 U _tgetent
                 U _tgetstr
                 U _tgoto
                 U _time
                 U _tputs
                 U _user_from_uid
                 U _uuid_unparse_upper
                 U _warn
                 U _warnx
                 U _wcwidth
                 U _write
                 U dyld_stub_binder

~/github.com/jjaniec/nm-otool
❯ ./ft_nm /bin/ls > a

~/github.com/jjaniec/nm-otool
❯ nm /bin/ls > b

~/github.com/jjaniec/nm-otool
❯ diff a b

```

```bash
~/github.com/jjaniec/nm-otool norme*
❯ ./ft_otool /bin/ls
/bin/ls:
Contents of (__TEXT,__text) section
0000000100000f08        55 48 89 e5 48 83 c7 68 48 83 c6 68 5d e9 62 36
0000000100000f18        00 00 55 48 89 e5 48 8d 46 68 48 8d 77 68 48 89
0000000100000f28        c7 5d e9 4d 36 00 00 55 48 89 e5 4c 8b 46 60 48
0000000100000f38        8b 57 60 48 8b 4a 30 b8 01 00 00 00 49 39 48 30
0000000100000f48        7f 1a 7d 07 b8 ff ff ff ff eb 11 48 8b 4a 38 49
0000000100000f58        39 48 38 7f 07 b8 ff ff ff ff 7d 02 5d c3 48 83
0000000100000f68        c7 68 48 83 c6 68 5d e9 08 36 00 00 55 48 89 e5
...

~/github.com/jjaniec/nm-otool
❯ ./ft_otool /bin/ls > a

~/github.com/jjaniec/nm-otool
❯ otool -t /bin/ls > b

~/github.com/jjaniec/nm-otool
❯ diff a b

```

### nm symbol types

![](https://www.reinterpretcast.com/images/mach_o_segments.gif?v=c40118745dc968157d1c3052f4bc9dd3)

|**Printed char**       |```U```|```A```|```T```|```D```|```B```|```C```|```-```|```S```|```I```|```W```|
|-----------------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|**Symbol Type**|undefined|absolute|text section symbol|data section  symbol|bss section symbol|common symbol|debugger symbols|symbol in another section|indirect symbol|Weak reference|

* Symbol Section obtained by checking the ```nlist.n_sect``` section's ```section.sectname``` (see section picture above)
* Additional Symbol Type info are in ```nlist.n_type```
    * If the symbol is local (non-external == ```N_EXT``` bit off), the symbol's type is instead represented by  the  corresponding lowercase  letter
    * A lower case u in a dynamic shared library indicates a undefined reference to a private external in another module in the same library
    * Weak references have ```nlist.n_desc & N_WEAK_REF```

## Documentation / Structures reference

- [Basic symbol parsing tutorial](https://lowlevelbits.org/parsing-mach-o-files/)

### Mach-O Headers and Magic

![](https://darkdonno.files.wordpress.com/2010/11/macho_header1.png)

#### ```magic``` number
```c
#define MH_MAGIC     0xfeedface    /* the mach magic number */
#define MH_CIGAM     0xcefaedfe    /* NXSwapInt(MH_MAGIC) */
#define MH_MAGIC_64  0xfeedfacf    /* the 64-bit mach magic number */
#define MH_CIGAM_64  0xcffaedfe    /* NXSwapInt(MH_MAGIC_64) */
#define FAT_MAGIC    0xcafebabe    /* the fat magic number */
#define FAT_CIGAM    0xbebafeca    /* NXSwapLong(FAT_MAGIC) */
#define FAT_MAGIC_64 0xcafebabf    /* the 64-bit fat magic number */
#define FAT_CIGAM_64 0xbfbafeca    /* NXSwapLong(FAT_MAGIC_64) */
```

#### ```mach_header``` and ```load_command```
```c
struct mach_header:             /* struct mach_header_64 for 64-bit */
    uint32_t        magic;      /* mach magic number identifier */
    cpu_type_t      cputype;    /* cpu specifier */
    cpu_subtype_t   cpusubtype; /* machine specifier */
    uint32_t        filetype;   /* type of file */
    uint32_t        ncmds;      /* number of load commands */
    uint32_t        sizeofcmds; /* the size of all the load commands */
    uint32_t        flags;      /* flags */
_64 uint32_t        reserved;   /* reserved */

struct load_command:            /* load_command header */
    uint32_t        cmd;        /* type of load command */
    uint32_t        cmdsize;    /* total size of command in bytes */
```

#### ```fat_header``` and ```fat_arch```

```c
struct fat_header:
    uint32_t        magic;      /* FAT_MAGIC or FAT_MAGIC_64 */
    uint32_t        nfat_arch;  /* number of structs that follow */

struct fat_arch:                /* struct fat_arch_64 for 64-bit*/
    cpu_type_t      cputype;    /* cpu specifier (int) */
    cpu_subtype_t   cpusubtype; /* machine specifier (int) */
_32 uint32_t        offset;     /* file offset to this object file */
_32 uint32_t        size;       /* size of this object file */
_64 uint64_t        offset;     /* file offset to this object file */
_64 uint64_t        size;       /* size of this object file */
    uint32_t        align;      /* alignment as a power of 2 */
_64 uint32_t        reserved;   /* reserved */
```

### Mach-O Segments and Sections

#### ```segment_command``` load_command
```c
struct segment_command:          /* struct segment_command_64 for 64-bit */
    uint32_t        cmd;         /* LC_SEGMENT or LC_SEGMENT_64 */
    uint32_t        cmdsize;     /* includes sizeof section structs */
    char            segname[16]; /* segment name */
_32 uint32_t        vmaddr;      /* memory address of this segment */
_32 uint32_t        vmsize;      /* memory size of this segment */
_32 uint32_t        fileoff;     /* file offset of this segment */
_32 uint32_t        filesize;    /* amount to map from the file */
_64 uint64_t        vmaddr;      /* memory address of this segment */
_64 uint64_t        vmsize;      /* memory size of this segment */
_64 uint64_t        fileoff;     /* file offset of this segment */
_64 uint64_t        filesize;    /* amount to map from the file */
    vm_prot_t       maxprot;     /* maximum VM protection */
    vm_prot_t       initprot;    /* initial VM protection */
    uint32_t        nsects;      /* number of sections in segment */
    uint32_t        flags;       /* flags */
```

#### ```section``` contained in a segment

```c
struct section:                   /* struct section_64 for 64-bit */
    char            sectname[16]; /* name of this section */
    char            segname[16];  /* segment this section goes in */
_64 uint64_t        addr;         /* for 64-bit memory address of this section */
_64 uint64_t        size;         /* for 64-bit size in bytes of this section */
_32 uint32_t        addr;         /* for 32-bit memory address of this section */
_32 uint32_t        size;         /* for 32-bit size in bytes of this section */
    uint32_t        offset;       /* file offset of this section */
    uint32_t        align;        /* section alignment (power of 2) */
    uint32_t        reloff;       /* file offset of relocation entries */
    uint32_t        nreloc;       /* number of relocation entries */
    uint32_t        flags;        /* flags (section type and attributes)*/
    uint32_t        reserved1;    /* reserved (for offset or index) */
    uint32_t        reserved2;    /* reserved (for count or sizeof) */
_64 uint32_t        reserved3;    /* reserved */
```

### Mach-O Symbols Segments

#### ```symtab_command``` load_command
```c
struct symtab_command:
    uint32_t        cmd;        /* LC_SYMTAB */
    uint32_t        cmdsize;    /* sizeof(struct symtab_command) */
    uint32_t        symoff;     /* symbol table offset (nlist array) */
    uint32_t        nsyms;      /* number of symbol table entries */
    uint32_t        stroff;     /* string table offset */
    uint32_t        strsize;    /* string table size in bytes */
```

#### Symbol ```nlist```
```c
struct nlist:                   /* struct nlist_64 for 64-bit */
    union           n_un:       /* union with char *n_name for 32-bit */
        uint32_t    n_strx;     /* index into the string table (union n_un) */
    uint8_t         n_type;     /* type flag, see below */
    uint8_t         n_sect;     /* section number or NO_SECT */
    int16_t         n_desc;     /* see <mach-o/stab.h> */
_32 uint32_t        n_value;    /* value of this symbol (or stab offset) */
_64 uint64_t        n_value;    /* value of this symbol (or stab offset) */
```
