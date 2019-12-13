/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/13 16:41:04 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

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

static int		parse_cmd_sectn_32(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct segment_command *seg32, uint32_t *sect_count)
{
	struct section		section;
	void				*section_ptr;
	uint32_t			i;

	i = 0;
	dprintf(2, "Segname64: %s\n", seg32->segname);
	section_ptr = (void *)(seg32 + 1);
	while (i < seg32->nsects)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section));
		dprintf(2, "Section %u / %u: %s - %s\n", i, seg32->nsects, section.segname, section.sectname);
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
		section_ptr += sizeof(struct section);
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
		if (hdrinfo->is_64)
			parse_cmd_sectn(file, hdrinfo, (struct segment_command_64 *)file->seek_ptr, &sect_count);
		else
			parse_cmd_sectn_32(file, hdrinfo, (struct segment_command *)file->seek_ptr, &sect_count);
		slseek(file, load_command_offset + load_command_size, SLSEEK_SET);
	}
	return (0);
}

/*
** Loop through syms and output with the correct format
*/

static void		dump_symlist(t_ft_nm_hdrinfo *hdrinfo, t_ft_nm_sym *symlist)
{
	t_ft_nm_sym		*s;

	s = symlist;
	while (s)
	{
		if (s->symtype != '-')
		{
			if (hdrinfo->is_64)
				ft_printf((s->symvalue != 0) ? \
					("%016" PRIx64 " %c %s\n") : ("%16c %c %s\n"), \
					(s->symvalue) ? (s->symvalue) : (' '), \
					s->symtype, s->symname);
			else
				ft_printf((s->symvalue != 0) ? \
					("%08" PRIx64 " %c %s\n") : ("%8c %c %s\n"), \
					(s->symvalue) ? (s->symvalue) : (' '), \
					s->symtype, s->symname);
		}
		s = s->next;
	}
}

static t_ft_nm_hdrinfo	*decide_hdr_to_use(t_ft_nm_hdrinfo *hdr_list)
{
	
}

int				ft_nm(t_ft_nm_file *file)
{
	t_ft_nm_hdrinfo		hdrinfo;
	t_ft_nm_hdrinfo		*hdr_to_use;
	int					idx;
	struct load_command	cmd;
	t_ft_nm_sym			*symlist;
	// uint32_t			load_command_size;

	if (init_header_info(file, &hdrinfo) == 1)
	{
		dprintf(2, "The file was not recognized as a valid object file\n");
		return (1);
	}
	slseek(file, hdrinfo.fat_offset + hdrinfo.machhdr_size, SLSEEK_SET);
	parse_file_segment_cmds(file, &hdrinfo, &cmd);
	dprintf(2, "text_nsect: %d - data_nsect: %d - bss_nsect: %d\n", hdrinfo.text_nsect, hdrinfo.data_nsect, hdrinfo.bss_nsect);
	slseek(file, hdrinfo.fat_offset + hdrinfo.machhdr_size, SLSEEK_SET);
	if ((idx = goto_load_command(file, &hdrinfo, (int [3]){LC_SYMTAB, 0}, &cmd)) != -1)
	{
		if (hdrinfo.is_64)
			symlist = build_symbol_list(\
				file, &hdrinfo, \
				(void *)file->seek_ptr - sizeof(struct load_command));
		else
			symlist = build_symbol_list_32(\
				file, &hdrinfo, \
				(void *)file->seek_ptr - sizeof(struct load_command));
		dump_symlist(&hdrinfo, symlist);
	}
	return (0);
}
