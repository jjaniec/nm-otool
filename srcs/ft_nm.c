/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/20 16:29:16 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		parse_cmd_sectn(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct segment_command_64 *seg64, uint32_t *sect_count)
{
	struct section_64	section;
	void				*section_ptr;
	uint32_t			i;
	uint32_t			parsed_size;

	i = 0;
	parsed_size = 0;
	dprintf(DEBUG_FD, "Segname64: %s - nsects: %u - size: %u - headersize: %zu - headersize section: %zu\n", seg64->segname, seg64->nsects, seg64->cmdsize, sizeof(struct segment_command_64), sizeof(struct section_64));
	section_ptr = (void *)(seg64 + 1);
	while (i < seg64->nsects && parsed_size < seg64->cmdsize)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section_64));
		dprintf(DEBUG_FD, "Section %u / %u: %s - %s\n", i, seg64->nsects, section.segname, section.sectname);
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
		parsed_size += sizeof(struct section_64);
		dprintf(DEBUG_FD, "LC Parsed size: %u / %u - ncmds: %u / %u\n", parsed_size, seg64->cmdsize, i, seg64->nsects);
		section_ptr += sizeof(struct section_64);
	}
	if (parsed_size + sizeof(struct segment_command_64) > seg64->cmdsize || i != seg64->nsects)
	{
		dprintf(ERR_FD, ERR_PREFIX "Inconsistent load command size / number of sections in LC_SEGMENT\n");
		return (1);
	}
	return (0);
}

static int		parse_cmd_sectn_32(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct segment_command *seg32, uint32_t *sect_count)
{
	struct section		section;
	void				*section_ptr;
	uint32_t			i;

	i = 0;
	dprintf(DEBUG_FD, "Segname32: %s\n", seg32->segname);
	section_ptr = (void *)(seg32 + 1);
	while (i < seg32->nsects)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section));
		dprintf(DEBUG_FD, "Section %u / %u: %s - %s\n", i, seg32->nsects, section.segname, section.sectname);
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
	slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	while ((idx = goto_load_command(file, hdrinfo, (int [3]){LC_SEGMENT, LC_SEGMENT_64}, cmd)) != -1)
	{
		load_command_size = cmd->cmdsize;
		load_command_offset = file->seek_ptr - file->content - sizeof(struct load_command);
		slseek(file, - ((int)sizeof(struct load_command)), SLSEEK_CUR);
		if (hdrinfo->is_64)
		{
			if (parse_cmd_sectn(file, hdrinfo, (struct segment_command_64 *)file->seek_ptr, &sect_count))
				return (1);
		}
		else
			parse_cmd_sectn_32(file, hdrinfo, (struct segment_command *)file->seek_ptr, &sect_count);
		slseek(file, load_command_offset + load_command_size, SLSEEK_SET);
	}
	return (0);
}

static t_ft_nm_hdrinfo	*goto_hdr_cpu_type(t_ft_nm_hdrinfo *hdr_list, cpu_type_t target_type)
{
	t_ft_nm_hdrinfo		*h;

	h = hdr_list;
	while (h && h->cpu_type != target_type)
		h = h->next;
	return (h);
}

int				ft_nm(t_ft_nm_file *file)
{
	t_ft_nm_hdrinfo		hdrs;
	t_ft_nm_hdrinfo		*hdr_to_use;
	int					idx;
	struct load_command	cmd;
	t_ft_nm_sym			*symlist;

	if (init_header_info(file, &hdrs) == 1)
	{
		dprintf(2, "The file was not recognized as a valid object file\n");
		return (1);
	}
	if (!(hdr_to_use = goto_hdr_cpu_type(&hdrs, HOST_CPU_TYPE)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_X86)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_I386)))
		return (1);
	if (check_load_commands(file, hdr_to_use) || parse_file_segment_cmds(file, hdr_to_use, &cmd))
		return (1);
	dprintf(DEBUG_FD, "text_nsect: %d - data_nsect: %d - bss_nsect: %d\n", hdr_to_use->text_nsect, hdr_to_use->data_nsect, hdr_to_use->bss_nsect);
	slseek(file, hdr_to_use->fat_offset + hdr_to_use->machhdr_size, SLSEEK_SET);
	if ((idx = goto_load_command(file, hdr_to_use, (int [3]){LC_SYMTAB, 0}, &cmd)) != -1)
	{
		if (hdr_to_use->is_64)
			symlist = build_symbol_list(\
				file, hdr_to_use, \
				(void *)file->seek_ptr - sizeof(struct load_command));
		else
			symlist = build_symbol_list_32(\
				file, hdr_to_use, \
				(void *)file->seek_ptr - sizeof(struct load_command));
		dump_symlist(hdr_to_use, symlist);
	}
	return (0);
}
