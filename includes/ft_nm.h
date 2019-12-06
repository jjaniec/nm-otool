/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:16 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 18:27:03 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H
# include <ft_printf.h>
# include <mach/machine.h>
# include <mach-o/loader.h>
# include <mach-o/swap.h>
# include <inttypes.h>

# define SLSEEK_CUR		1
# define SLSEEK_SET		2

// struct mach_header {
// 	uint32_t		magic;
// 	cpu_type_t		cputype;
// 	cpu_subtype_t	cpusubtype;
// 	uint32_t		filetype;
// 	uint32_t		ncmds;
// 	uint32_t		sizeofcmds;
// 	uint32_t		flags;
// };

// struct segment_command {
// 	uint32_t		cmd;
// 	uint32_t		cmdsize;
// 	char			segname[16];
// 	uint32_t		vmaddr;
// 	uint32_t		vmsize;
// 	uint32_t		fileoff;
// 	uint32_t		filesize;
// 	vm_prot_t		maxprot;
// 	vm_prot_t		initprot;
// 	uint32_t		nsects;
// 	uint32_t		flags;
// };

// struct load_command {
//   uint32_t cmd;
//   uint32_t cmdsize;
// };

// struct segment_command {
// 	uint32_t 	cmd
// 	uint32_t 	cmdsize
// 	uint32_t 	fileoff
// 	uint32_t 	filesize
// 	uint32_t 	flags
// 	vm_prot_t 	initprot
// 	vm_prot_t 	maxprot
// 	uint32_t 	nsects
// 	char 	segname [16]
// 	uint32_t 	vmaddr
// 	uint32_t 	vmsize
// }

// struct section
// {
// 	char sectname[16];
// 	char segname[16];
// 	uint32_t addr;
// 	uint32_t size;
// 	uint32_t offset;
// 	uint32_t align;
// 	uint32_t reloff;
// 	uint32_t nreloc;
// 	uint32_t flags;
// 	uint32_t reserved1;
// 	uint32_t reserved2;
// };

// struct symtab_command
// {
// 	uint32_t cmd;
// 	uint32_t cmdsize;
// 	uint32_t symoff;
// 	uint32_t nsyms;
// 	uint32_t stroff;
// 	uint32_t strsize;
// };

//	struct nlist_64 {
//		uint32_t n_strx;
//		uint8_t n_type;
//		uint8_t n_sect;
//		uint16_t n_desc;
//		uint64_t n_value;
//	};

typedef struct s_ft_nm_file
{
	size_t totsiz;
	char *seek_ptr;
	char *content;
} t_ft_nm_file;

typedef struct	s_ft_nm_hdrinfo {
	t_ft_nm_file		*file;
	uint32_t			magic;
	bool				is_64;
	bool				is_be; // is big endian
	size_t				machhdr_size;
	uint32_t			ncmds;
	uint8_t				text_nsect;
	uint8_t				data_nsect;
	uint8_t				bss_nsect;
}				t_ft_nm_hdrinfo;

typedef struct	s_ft_nm_sym {
	uint64_t			symvalue;
	char				*symname;
	char				symtype;
	struct s_ft_nm_sym	*next;
}				t_ft_nm_sym;


int					ft_nm(t_ft_nm_file *file);

bool				is_magic_64(uint32_t magic);

bool				is_big_endian(uint32_t magic);

void				swap_byte_range(void *bytes, size_t range);

int					init_header_info(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo);

int					sseek_read(t_ft_nm_file *file, void *buf, unsigned int size);

off_t				slseek(t_ft_nm_file *file, off_t offset, int whence);

int					goto_load_command(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, int load_cmds[2], struct load_command *cmd);

t_ft_nm_sym			*build_symbol_list(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo, struct symtab_command *symtabcmd);

#endif
