/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm_process_file.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/15 18:37:56 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		assign_cmd_nsect(t_ft_nm_hdrinfo *hdrinfo, \
					uint32_t *sect_count, void *section_ptr)
{
	char		*sectname;
	char		*segname;

	if (hdrinfo->is_64)
	{
		sectname = ((struct section_64 *)section_ptr)->sectname;
		segname = ((struct section_64 *)section_ptr)->segname;
	}
	else
	{
		sectname = ((struct section *)section_ptr)->sectname;
		segname = ((struct section *)section_ptr)->segname;
	}
	if (ft_strcmp(sectname, SECT_TEXT) == 0 &&
		ft_strcmp(segname, SEG_TEXT) == 0)
		hdrinfo->text_nsect = *sect_count + 1;
	else if (ft_strcmp(sectname, SECT_DATA) == 0 &&
		ft_strcmp(segname, SEG_DATA) == 0)
		hdrinfo->data_nsect = *sect_count + 1;
	else if (ft_strcmp(sectname, SECT_BSS) == 0 &&
		ft_strcmp(segname, SEG_DATA) == 0)
		hdrinfo->bss_nsect = *sect_count + 1;
	*sect_count += 1;
	return (0);
}

static int		parse_cmd_sectn(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command_64 *seg64, uint32_t *sect_count)
{
	struct section_64	section;
	void				*section_ptr;
	uint32_t			i;
	uint32_t			parsed_size;

	i = 0;
	parsed_size = 0;
	section_ptr = (void *)(seg64 + 1);
	while (i < seg64->nsects && parsed_size < seg64->cmdsize)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section_64));
		assign_cmd_nsect(hdrinfo, sect_count, &section);
		i++;
		parsed_size += sizeof(struct section_64);
		section_ptr += sizeof(struct section_64);
	}
	if (parsed_size + sizeof(struct segment_command_64) > \
		seg64->cmdsize || i != seg64->nsects)
	{
		ft_putstr_fd(NM_ERR_PREFIX "Inconsistent load command size" \
			" / number of sections in LC_SEGMENT\n", ERR_FD);
		return (1);
	}
	return (0);
}

static int		parse_cmd_sectn_32(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command *seg32, uint32_t *sect_count)
{
	struct section		section;
	void				*section_ptr;
	uint32_t			i;
	uint32_t			parsed_size;

	i = 0;
	parsed_size = 0;
	section_ptr = (void *)(seg32 + 1);
	while (i < seg32->nsects && parsed_size < seg32->cmdsize)
	{
		slseek(file, section_ptr - (void *)file->content, SLSEEK_SET);
		sseek_read(file, &section, sizeof(struct section));
		assign_cmd_nsect(hdrinfo, sect_count, &section);
		i++;
		parsed_size += sizeof(struct section);
		section_ptr += sizeof(struct section);
	}
	if (parsed_size + sizeof(struct segment_command_64) > \
		seg32->cmdsize || i != seg32->nsects)
	{
		ft_putstr_fd(NM_ERR_PREFIX "Inconsistent load command size" \
			" / number of sections in LC_SEGMENT\n", ERR_FD);
		return (1);
	}
	return (0);
}

static int		parse_file_segment_cmds(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo, struct load_command *cmd)
{
	uint32_t			load_command_size;
	uint32_t			load_command_offset;
	int					idx;
	uint32_t			sect_count;

	sect_count = 0;
	slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	while ((idx = goto_load_command(file, hdrinfo, \
		(uint32_t[3]){LC_SEGMENT, LC_SEGMENT_64}, cmd)) != -1)
	{
		load_command_size = cmd->cmdsize;
		load_command_offset = file->seek_ptr - file->content - \
			sizeof(struct load_command);
		slseek(file, -((int)sizeof(struct load_command)), SLSEEK_CUR);
		if (hdrinfo->is_64 && parse_cmd_sectn(file, hdrinfo, \
			(struct segment_command_64 *)file->seek_ptr, &sect_count))
			return (1);
		else if (!hdrinfo->is_64)
			parse_cmd_sectn_32(file, hdrinfo, \
				(struct segment_command *)file->seek_ptr, &sect_count);
		slseek(file, load_command_offset + load_command_size, SLSEEK_SET);
	}
	return (0);
}

int				ft_nm_process_file(t_ft_nm_file *file, bool print_filename)
{
	t_ft_nm_hdrinfo		hdrs;
	t_ft_nm_hdrinfo		*hdr_to_use;
	struct load_command	cmd;
	t_ft_nm_sym			*symlist;

	if (init_header_info(file, &hdrs) >= 1)
		return (1);
	if (!(hdr_to_use = goto_hdr_cpu_type(&hdrs, HOST_CPU_TYPE)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_X86)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_I386)))
		return (1);
	if (check_load_commands(file, hdr_to_use) || \
		parse_file_segment_cmds(file, hdr_to_use, &cmd))
		return (1);
	if (!(symlist = build_symbol_list(hdr_to_use, true)))
		return (1);
	dump_symlist(hdr_to_use, symlist, print_filename);
	free_symbol_list(symlist);
	return (0);
}
