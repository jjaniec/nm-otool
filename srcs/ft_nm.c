/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/30 22:04:19 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			sseek_read(t_ft_nm_file *file, void *buf, unsigned int size)
{
	unsigned int	i;

	i = 0;
	while (file->seek_ptr < (file->content + file->totsiz) && i < size)
	{
		*((unsigned char *)buf + i) = *((unsigned char *)file->seek_ptr);
		file->seek_ptr += 1;
		i++;
	}
	return (i);
}

static off_t		slseek(t_ft_nm_file *file, off_t offset, int whence)
{
	off_t	r;

	r = -1;
	if (whence == SLSEEK_CUR)
	{
		if ((offset <= 0 && (size_t)offset < (size_t)((*file).seek_ptr - (*file).content)) || \
			((size_t)offset + (size_t)((*file).seek_ptr - (*file).content) <= (*file).totsiz))
		{
			(*file).seek_ptr += offset;
			r = offset;
		}
	}
	else if (whence == SLSEEK_SET)
	{
		if (offset >= 0 && (*file).totsiz >= (size_t)offset)
		{
			(*file).seek_ptr = (*file).content + offset;
			r = offset;
		}
	}
	return (r);
}

static uint32_t	get_ncmds(t_ft_nm_file *file, t_nm_info *fileinfo)
{
	struct mach_header		hdr;
	struct mach_header_64	hdr64;

	slseek(file, 0, SLSEEK_SET);
	printf("lseek offset: %ld\n", file->seek_ptr - file->content);
	if (fileinfo->is_64)
	{
		sseek_read(file, &hdr64, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr64, fileinfo->machhdr_size);
		printf("Mach header 64:\nmagic\t\tcputype\t\tcpusubtype\tfiletype\tncmds\t\tsizeofcmds\tflags\n%x\t%u\t%u\t%u\t\t%u\t\t%u\t\t%x\n", hdr64.magic, hdr64.cputype, hdr64.cpusubtype, hdr64.filetype, hdr64.ncmds, hdr64.sizeofcmds, hdr64.flags);
		return (hdr64.ncmds);
	}
	else
	{
		sseek_read(file, &hdr, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr, fileinfo->machhdr_size);
		return (hdr.ncmds);
	}
	return (0);
}

static int		init_file_info(t_ft_nm_file *file, t_nm_info *fileinfo)
{
	uint32_t			magic;

	sseek_read(file, &magic, sizeof(uint32_t));
	fileinfo->is_64 = is_magic_64(magic);
	fileinfo->is_be = is_big_endian(magic);
	fileinfo->magic = magic;
	fileinfo->machhdr_size = (fileinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	if ((fileinfo->ncmds = get_ncmds(file, fileinfo)) == 0)
		return (-1);
	return (0);
}

static void		dump_sections(t_ft_nm_file *file, t_nm_info *fileinfo, struct segment_command_64 *seg64)
{
	struct section_64	section;
	void			*section_ptr;
	uint32_t		i;

	i = 0;
	section_ptr = (void *)(seg64 + 1);
	while (i < seg64->nsects)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section_64));
		printf("Section: %s - %s\n", section.segname, section.sectname);
		i++;
		section_ptr += sizeof(struct section_64);
	}	
}

static char		*get_symbol_type(uint32_t type)
{
	if (type == N_UNDF)
		return "U";
	if (type == N_ABS)
		return "TODO";
	if (type == N_SECT)
		return "T";
	if (type == N_PBUD)
		return "TODO";
	if (type == N_INDR)
		return "TODO";
	return NULL;
}

static void		dump_syms(t_ft_nm_file *file, t_nm_info *fileinfo, struct symtab_command *symtabcmd)
{
	uint32_t			i;
	struct nlist_64		*symtab;
	struct nlist_64		nl;
	const char* strtab;

	i = 0;
	strtab = symtabcmd->stroff + (char *)file->content;
	slseek(file, symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist_64 *)file->seek_ptr;
	printf("%u symtabcmd offset - nsysms: %u\n", symtabcmd->symoff, symtabcmd->nsyms);
	while (i < symtabcmd->nsyms)
	{
		slseek(file, &symtab[i], SLSEEK_SET);
		sseek_read(file, &nl, sizeof(struct nlist_64));
		i++;
		if(nl.n_type & N_STAB) {
			continue;
		}
		const char* type = get_symbol_type(nl.n_type & N_TYPE);
		const char* symname = &strtab[nl.n_un.n_strx];
		printf("Symbol \"%s\" type: %s (%x) value: 0x%llx\n", symname, type, nl.n_type, nl.n_value);
	}
}

static void		dump_cmds(t_ft_nm_file *file, t_nm_info *fileinfo)
{
	uint32_t					i = 0;
	struct load_command			cmd;
	struct segment_command		seg;
	struct segment_command_64	seg64;
	off_t						seek_ptr_save;

	slseek(file, fileinfo->machhdr_size, SLSEEK_SET);
	while (i < fileinfo->ncmds)
	{
		seek_ptr_save = file->seek_ptr - file->content;
		sseek_read(file, &cmd, sizeof(struct load_command));
		if (!fileinfo->is_be)
			swap_byte_range(&cmd, sizeof(struct load_command));
		printf("seek: %d\n", file->seek_ptr - file->content);
		printf("Load command size: %u\n", cmd.cmdsize);
		slseek(file, - sizeof(struct load_command), SLSEEK_CUR);
		if (cmd.cmd == LC_SEGMENT_64)
		{
			sseek_read(file, &seg64, sizeof(struct segment_command_64));
			if (!fileinfo->is_be)
				swap_byte_range(&seg64, sizeof(struct segment_command_64));
			printf("Segname64: %s\n", seg64.segname);
			dump_sections(file, fileinfo, &seg64);
		}
		else if (cmd.cmd == LC_SEGMENT)
		{
			sseek_read(file, &seg, sizeof(struct segment_command));
			if (!fileinfo->is_be)
				swap_byte_range(&seg, sizeof(struct segment_command));
			printf("Segname: %s\n", seg.segname);
		}
		if (cmd.cmd == LC_SYMTAB)
			dump_syms(file, fileinfo, file->content + seek_ptr_save);
		// slseek(file, seek_ptr_save + sizeof(struct segment_command), SLSEEK_SET);
		slseek(file, seek_ptr_save + cmd.cmdsize, SLSEEK_SET);
		i++;
	}
}

int				ft_nm(t_ft_nm_file *file)
{
	t_nm_info		fileinfo;

	printf("Filesize: %zu\n", file->totsiz);
	init_file_info(file, &fileinfo);
	dump_cmds(file, &fileinfo);
	return (0);
}
