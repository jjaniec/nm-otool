/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/07 17:46:01 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

// static void		dump_cmds(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo)
// {
// 	uint32_t					i = 0;
// 	struct load_command			cmd;
// 	struct segment_command		seg;
// 	struct segment_command_64	seg64;
// 	off_t						seek_ptr_save;

// 	slseek(file, fileinfo->machhdr_size, SLSEEK_SET);
// 	while (i < fileinfo->ncmds)
// 	{
// 		seek_ptr_save = file->seek_ptr - file->content;
// 		sseek_read(file, &cmd, sizeof(struct load_command));
// 		if (!fileinfo->is_be)
// 			swap_byte_range(&cmd, sizeof(struct load_command));
// 		printf("seek: %d\n", file->seek_ptr - file->content);
// 		printf("Load command size: %u\n", cmd.cmdsize);
// 		slseek(file, - sizeof(struct load_command), SLSEEK_CUR);
// 		if (cmd.cmd == LC_SEGMENT_64)
// 		{
// 			sseek_read(file, &seg64, sizeof(struct segment_command_64));
// 			if (!fileinfo->is_be)
// 				swap_byte_range(&seg64, sizeof(struct segment_command_64));
// 			printf("Segname64: %s\n", seg64.segname);
// 			dump_sections(file, fileinfo, &seg64);
// 		}
// 		else if (cmd.cmd == LC_SEGMENT)
// 		{
// 			sseek_read(file, &seg, sizeof(struct segment_command));
// 			if (!fileinfo->is_be)
// 				swap_byte_range(&seg, sizeof(struct segment_command));
// 			printf("Segname: %s\n", seg.segname);
// 		}
// 		if (cmd.cmd == LC_SYMTAB)
// 			dump_syms(file, fileinfo, file->content + seek_ptr_save);
// 		slseek(file, seek_ptr_save + cmd.cmdsize, SLSEEK_SET);
// 		i++;
// 	}
// }

// static int		dump_sections(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo, struct segment_command_64 *seg64)
// {
// 	struct section_64	section;
// 	void			*section_ptr;
// 	uint32_t		i;

// 	i = 0;
// 	printf("Segname64: %s\n", seg64->segname);
// 	section_ptr = (void *)(seg64 + 1);
// 	while (i < seg64->nsects)
// 	{
// 		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
// 		sseek_read(file, &section, sizeof(struct section_64));
// 		printf("Section: %s - %s\n", section.segname, section.sectname);
// 		i++;
// 		section_ptr += sizeof(struct section_64);
// 	}
// 	return (0);
// }

static int		parse_cmd_sectn(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct segment_command_64 *seg64, uint32_t *sect_count)
{
	struct section_64	section;
	void				*section_ptr;
	uint32_t			i;

	i = 0;
	dprintf(2, "Segname64: %s\n", seg64->segname);
	section_ptr = (void *)(seg64 + 1);
	while (i < seg64->nsects)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section_64));
		dprintf(2, "Section %u / %u: %s - %s\n", i, seg64->nsects, section.segname, section.sectname);
		if (strcmp(section.sectname, SECT_TEXT) == 0 &&
			strcmp(section.segname, SEG_TEXT) == 0)
			hdrinfo->text_nsect = *sect_count + 1;
		else if (strcmp(section.sectname, SECT_DATA) == 0 &&
			strcmp(section.segname, SEG_DATA) == 0)
			hdrinfo->data_nsect = *sect_count + 1;
		else if (strcmp(section.sectname, SECT_BSS) == 0 &&
			strcmp(section.segname, SEG_DATA) == 0)
			hdrinfo->bss_nsect = *sect_count + 1;
		i++;
		*sect_count += 1;
		section_ptr += sizeof(struct section_64);
	}
	return (0);

}

static int		parse_file_segment_cmds(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct load_command *cmd)
{
	uint32_t			load_command_size;
	uint32_t			load_command_offset;
	int					idx;
	uint32_t			sect_count;

	sect_count = 0;
	while ((idx = goto_load_command(file, hdrinfo, (int [3]){LC_SEGMENT, LC_SEGMENT_64}, cmd)) != -1)
	{
		load_command_size = cmd->cmdsize;
		load_command_offset = file->seek_ptr - file->content - sizeof(struct load_command);
		slseek(file, - sizeof(struct load_command), SLSEEK_CUR);
		parse_cmd_sectn(file, hdrinfo, file->seek_ptr, &sect_count);
		slseek(file, load_command_offset + load_command_size, SLSEEK_SET);
	}
	return (0);
}

/*
** Loop through syms and output with the correct format
*/

static void		dump_symlist(t_ft_nm_sym *symlist)
{
	t_ft_nm_sym		*s;

	s = symlist;
	while (s)
	{
		if (s->symtype != '-')
			ft_printf((s->symvalue != 0) ? \
				("%016" PRIx64 " %c %s\n") : ("%16c %c %s\n"), \
				(s->symvalue) ? (s->symvalue) : (' '), \
				s->symtype, s->symname);
		s = s->next;
	}
}

int				ft_nm(t_ft_nm_file *file)
{
	t_ft_nm_hdrinfo		hdrinfo;
	int					idx;
	struct load_command	cmd;
	t_ft_nm_sym			*symlist;
	uint32_t			load_command_size;

	if (init_header_info(file, &hdrinfo) == -1)
	{
		dprintf(2, "The file was not recognized as a valid object file\n");
		return (1);
	}
	slseek(file, hdrinfo.offset + hdrinfo.machhdr_size, SLSEEK_SET);
	parse_file_segment_cmds(file, &hdrinfo, &cmd);
	dprintf(2, "text_nsect: %d - data_nsect: %d - bss_nsect: %d\n", hdrinfo.text_nsect, hdrinfo.data_nsect, hdrinfo.bss_nsect);
	slseek(file, hdrinfo.offset + hdrinfo.machhdr_size, SLSEEK_SET);
	if ((idx = goto_load_command(file, &hdrinfo, (int [3]){LC_SYMTAB, 0}, &cmd)) != -1)
	{
		symlist = build_symbol_list(\
			file, &hdrinfo, \
			(void *)file->seek_ptr - sizeof(struct load_command));
		dump_symlist(symlist);
	}
	return (0);
}
