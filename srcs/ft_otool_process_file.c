/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_otool_process_file.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 18:20:25 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/13 22:09:43 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static t_ft_otool_sect	*get_sect_data(t_ft_nm_hdrinfo *hdrinfo, \
							struct section_64 *section, \
							char *sectname, char *segname)
{
	t_ft_otool_sect	*r;

	if (ft_strcmp((hdrinfo->is_64) ? (section->sectname) : \
			(((struct section *)section)->sectname), sectname) == 0 && \
		ft_strcmp((hdrinfo->is_64) ? (section->segname) : \
			(((struct section *)section)->segname), segname) == 0 && \
		(r = ft_memalloc(sizeof(t_ft_otool_sect))))
	{
		r->offset = (hdrinfo->is_64) ? (section->offset) : \
			(((struct section *)section)->offset);
		r->size = (hdrinfo->is_64) ? (section->size) : \
			(((struct section *)section)->size);
		r->address = (hdrinfo->is_64) ? (section->addr) : \
			(((struct section *)section)->addr);
		return (r);
	}
	return (NULL);
}

static t_ft_otool_sect	*search_sect(t_ft_nm_hdrinfo *hdrinfo, \
							struct segment_command_64 *seg, \
							char *sectname, char *segname)
{
	struct section_64	section;
	uint32_t			i;
	t_ft_otool_sect		*r;
	uint32_t			parsed_size;

	i = 0;
	parsed_size = 0;
	while (i++ < ((hdrinfo->is_64) ? (seg->nsects) : \
			(((struct segment_command *)seg)->nsects)) && \
		parsed_size < ((hdrinfo->is_64) ? (seg->cmdsize) : \
			(((struct segment_command *)seg))->cmdsize))
	{
		slseek(hdrinfo->file, (parsed_size + (void *)((hdrinfo->is_64) ? \
			(seg + 1) : ((struct segment_command *)seg + 1))) - \
			(void *)hdrinfo->file->content, SLSEEK_SET);
		sseek_read(hdrinfo->file, &section, sizeof(struct section_64));
		if ((r = get_sect_data(hdrinfo, &section, sectname, segname)))
			return (r);
		parsed_size += (hdrinfo->is_64) ? \
			(sizeof(struct section_64)) : (sizeof(struct section));
	}
	if (parsed_size + sizeof(struct segment_command_64) > seg->cmdsize || \
		i != seg->nsects)
		ft_putstr_fd(OTOOL_ERR_PREFIX "Inconsistent load command size" \
			" / number of sections in LC_SEGMENT\n", ERR_FD);
	return (NULL);
}

static t_ft_otool_sect	*get_text_sect_offset(t_ft_nm_hdrinfo *hdrinfo)
{
	struct load_command	cmd;
	uint32_t			load_command_size;
	uint32_t			load_command_offset;
	int					idx;
	t_ft_otool_sect		*text_sect;

	slseek(hdrinfo->file, hdrinfo->fat_offset + \
		hdrinfo->machhdr_size, SLSEEK_SET);
	while ((idx = goto_load_command(hdrinfo->file, hdrinfo, \
		(uint32_t[3]){LC_SEGMENT, LC_SEGMENT_64}, &cmd)) != -1)
	{
		load_command_size = cmd.cmdsize;
		load_command_offset = hdrinfo->file->seek_ptr - \
			hdrinfo->file->content - sizeof(struct load_command);
		slseek(hdrinfo->file, -((int)sizeof(struct load_command)), SLSEEK_CUR);
		if ((text_sect = search_sect(hdrinfo, \
			(void *)hdrinfo->file->seek_ptr, SECT_TEXT, SEG_TEXT)))
		{
			text_sect->hdr = hdrinfo;
			break ;
		}
		slseek(hdrinfo->file, load_command_offset + \
			load_command_size, SLSEEK_SET);
	}
	return (text_sect);
}

static int				dump_text_section(t_ft_nm_hdrinfo *hdrinfo, \
							t_ft_otool_sect *text_section)
{
	uint32_t			n;
	uint32_t			byte_n;
	char				byte_val;

	n = 0;
	ft_putstr_fd("Contents of (__TEXT,__text) section\n", STDOUT_FILENO);
	while (n < text_section->size)
	{
		if (hdrinfo->is_64)
			ft_printf("%016" PRIx64 "%c", text_section->address + n, '\t');
		else
			ft_printf("%08" PRIx32 "%c", text_section->address + n, '\t');
		byte_n = 0;
		while (byte_n < 16 && n + byte_n < text_section->size)
		{
			if (read_byte_range_at_pos(hdrinfo, &byte_val, sizeof(char), \
				hdrinfo->fat_offset + text_section->offset + n + byte_n))
				return (1);
			ft_printf("%02" PRIx8 " ", byte_val);
			byte_n++;
		}
		ft_printf("\n");
		n += 16;
	}
	return (0);
}

int						ft_otool_process_file(t_ft_nm_file *file)
{
	t_ft_nm_hdrinfo		hdrs;
	t_ft_nm_hdrinfo		*hdr_to_use;
	t_ft_otool_sect		*text_section;
	t_ft_nm_sym			*syms;
	int					r;

	if ((r = init_header_info(file, &hdrs)) >= 1)
		return (1);
	if (!(hdr_to_use = goto_hdr_cpu_type(&hdrs, HOST_CPU_TYPE)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_X86)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_I386)))
		return (1);
	if (check_load_commands(file, hdr_to_use) || \
		!(syms = build_symbol_list(hdr_to_use, false)))
		return (1);
	free_symbol_list(syms);
	ft_printf("%s:\n", file->filepath);
	if (!((text_section = get_text_sect_offset(hdr_to_use))))
		return (0);
	if (slseek(file, hdr_to_use->fat_offset + \
		text_section->offset, SLSEEK_SET) == -1)
		return (1);
	dump_text_section(hdr_to_use, text_section);
	free(text_section);
	return (0);
}
