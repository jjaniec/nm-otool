/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:16 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 19:38:40 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H
# include <ft_printf.h>
# include <mach/machine.h>
# include <mach-o/loader.h>
# include <mach-o/swap.h>

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

typedef struct s_ft_nm_file
{
	size_t totsiz;
	char *seek_ptr;
	char *content;
} t_ft_nm_file;

typedef struct	s_ft_nm_info {
	t_ft_nm_file		*file;
	uint32_t			magic;
	bool				is_64;
	bool				is_be; // is big endian
	size_t				machhdr_size;
	uint32_t			ncmds;
}				t_nm_info;

int					ft_nm(t_ft_nm_file *file);

bool				is_magic_64(uint32_t magic);

bool				is_big_endian(uint32_t magic);

void				swap_byte_range(void *bytes, size_t range);

#endif
