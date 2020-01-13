/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:16 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/13 22:19:43 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H
# include <ft_printf.h>
# include <mach/machine.h>
# include <mach-o/loader.h>
# include <mach-o/swap.h>
# include <inttypes.h>

# define HOST_CPU_TYPE		CPU_TYPE_X86_64

# define SLSEEK_CUR			1
# define SLSEEK_SET			2

# define ERR_HDR_OVERLAP	-2

# define ERR_FD				2
# define OTOOL_ERR_PREFIX	"ft_otool: "
# define NM_ERR_PREFIX		"ft_nm: "
# define DEBUG_FD			2

/*
**	struct mach_header {
**		uint32_t		magic;
**		cpu_type_t		cputype;
**		cpu_subtype_t	cpusubtype;
**		uint32_t		filetype;
**		uint32_t		ncmds;
**		uint32_t		sizeofcmds;
**		uint32_t		flags;
**	};
**
**	struct segment_command {
**		uint32_t		cmd;
**		uint32_t		cmdsize;
**		char			segname[16];
**		uint32_t		vmaddr;
**		uint32_t		vmsize;
**		uint32_t		fileoff;
**		uint32_t		filesize;
**		vm_prot_t		maxprot;
**		vm_prot_t		initprot;
**		uint32_t		nsects;
**		uint32_t		flags;
**	};
**
**	struct load_command {
**	  uint32_t cmd;
**	  uint32_t cmdsize;
**	};
**
**	struct segment_command {
**		uint32_t 	cmd
**		uint32_t 	cmdsize
**		uint32_t 	fileoff
**		uint32_t 	filesize
**		uint32_t 	flags
**		vm_prot_t 	initprot
**		vm_prot_t 	maxprot
**		uint32_t 	nsects
**		char 	segname [16]
**		uint32_t 	vmaddr
**		uint32_t 	vmsize
**	}
**
**	struct section
**	{
**		char sectname[16];
**		char segname[16];
**		uint32_t addr;
**		uint32_t size;
**		uint32_t offset;
**		uint32_t align;
**		uint32_t reloff;
**		uint32_t nreloc;
**		uint32_t flags;
**		uint32_t reserved1;
**		uint32_t reserved2;
**	};
**
**	struct symtab_command
**	{
**		uint32_t cmd;
**		uint32_t cmdsize;
**		uint32_t symoff;
**		uint32_t nsyms;
**		uint32_t stroff;
**		uint32_t strsize;
**	};
**
**	struct nlist_64 {
**		uint32_t n_strx;
**		uint8_t n_type;
**		uint8_t n_sect;
**		uint16_t n_desc;
**		uint64_t n_value;
**	};
**
**	struct fat_arch {
**	    cpu_type_t  cputype;    cpu specifier (int)
**	    cpu_subtype_t   cpusubtype; machine specifier (int)
**	    uint32_t    offset;     file offset to this object file
**	    uint32_t    size;       size of this object file
**	    uint32_t    align;      alignment as a power of 2
**	};
**
**	struct fat_header {
**	    uint32_t    magic;      FAT_MAGIC or FAT_MAGIC_64
**	    uint32_t    nfat_arch;  number of structs that follow
**	};
*/

typedef struct				s_ft_nm_file
{
	char					*filepath;
	char					*seek_ptr;
	char					*content;
	int64_t					totsiz;
}							t_ft_nm_file;

typedef struct				s_ft_nm_hdrinfo {
	t_ft_nm_file			*file;
	struct s_ft_nm_hdrinfo	*next;
	uint32_t				fat_size;
	uint32_t				fat_align;
	uint32_t				fat_offset;
	uint32_t				magic;
	cpu_type_t				cpu_type;
	cpu_subtype_t			cpu_subtype;
	size_t					machhdr_size;
	uint32_t				ncmds;
	uint32_t				sizeofcmds;
	uint8_t					text_nsect;
	uint8_t					data_nsect;
	uint8_t					bss_nsect;
	bool					is_64;
	bool					is_be;
}							t_ft_nm_hdrinfo;

typedef struct				s_ft_nm_sym {
	struct s_ft_nm_sym		*next;
	uint64_t				symvalue;
	char					*symname;
	char					*indr_name;
	uint8_t					n_type;
	char					symtype;
}							t_ft_nm_sym;

int							ft_nm_process_file(t_ft_nm_file *file, \
								bool print_filename);

bool						is_magic_64(uint32_t magic);

bool						is_magic_fat(uint32_t magic);

bool						is_magic_mach(uint32_t magic);

bool						is_big_endian(uint32_t magic);

int							init_header_info(t_ft_nm_file *file, \
								t_ft_nm_hdrinfo *fileinfo);

int							goto_load_command(t_ft_nm_file *file, \
								t_ft_nm_hdrinfo *hdrinfo, \
								uint32_t load_cmds[2], \
								struct load_command *cmd);

t_ft_nm_sym					*build_symbol_list(t_ft_nm_hdrinfo *hdrinfo, \
								bool handle_corrupt_syms);

uint32_t					swap_32bit(uint32_t x);

int							check_hdr_overlap(t_ft_nm_hdrinfo *hdrinfo, \
								int offset);

int							check_load_commands(t_ft_nm_file *file, \
								t_ft_nm_hdrinfo *hdrinfo);

void						dump_symlist(t_ft_nm_hdrinfo *hdrinfo, \
								t_ft_nm_sym *symlist, bool print_filename);

int							read_file_content(char *file, \
								char **content);

t_ft_nm_hdrinfo				*goto_hdr_cpu_type(t_ft_nm_hdrinfo *hdr_list, \
								cpu_type_t target_type);

void						*free_symbol_list(t_ft_nm_sym *syms);

t_ft_nm_sym					*add_new_sym_to_list(t_ft_nm_sym **list, \
								uint64_t symvalue, char **symname, \
								char *types);

int							check_segment_command(t_ft_nm_hdrinfo *hdrinfo, \
								void *seg, uint32_t *parsed_filesize);

/*
** Parsing
*/

int							read_byte_range_at_pos(t_ft_nm_hdrinfo *hdrinfo, \
								void *buffer, unsigned int size, \
								int offset);

int							sseek_read(t_ft_nm_file *file, void *buf, \
								unsigned int size);

int							slseek(t_ft_nm_file *file, int offset, int whence);

void						swap_byte_range(void *bytes, size_t range);

/*
** otool
*/

typedef struct				s_ft_otool_sect {
	uint32_t				size;
	uint64_t				address;
	uint32_t				offset;
	t_ft_nm_hdrinfo			*hdr;
}							t_ft_otool_sect;

int							ft_otool_process_file(t_ft_nm_file *file);

#endif
