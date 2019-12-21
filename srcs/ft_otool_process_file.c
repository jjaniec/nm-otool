/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_otool_process_file.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joffreyjaniec <joffreyjaniec@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 18:20:25 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/21 15:38:28 by joffreyjani      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static t_ft_otool_sect	*search_sect(t_ft_nm_hdrinfo *hdrinfo, struct segment_command_64 *seg, char *sectname, char *segname)
{
	struct section_64	section;
	void				*section_ptr;
	uint32_t			i;
	uint32_t			parsed_size;

	i = 0;
	parsed_size = 0;
	dprintf(DEBUG_FD, "Segname32: %s\n", seg->segname);
	section_ptr = (void *)(seg + 1);
	while (i < seg->nsects && parsed_size < seg->cmdsize)
	{
		slseek(hdrinfo->file, section_ptr - (void *)hdrinfo->file->content, SLSEEK_SET);
		sseek_read(hdrinfo->file, &section, sizeof(struct section_64));
		dprintf(DEBUG_FD, "Section %u / %u: %s - %s - offset: %u - size: %u\n", i, seg->nsects, section.segname, section.sectname, section.offset, section.size);
		if (ft_strcmp(section.sectname, sectname) == 0 &&
			ft_strcmp(section.segname, segname) == 0)
		{
			section_ptr = malloc(sizeof(t_ft_otool_sect));
			((t_ft_otool_sect *)section_ptr)->offset = section.offset;
			((t_ft_otool_sect *)section_ptr)->size = section.size;
			return (section_ptr);
		}
		i++;
		parsed_size += sizeof(struct section_64);
		section_ptr += sizeof(struct section_64);
	}
	if (parsed_size + sizeof(struct segment_command_64) > seg->cmdsize || i != seg->nsects)
		ft_putstr_fd(ERR_PREFIX "Inconsistent load command size / number of sections in LC_SEGMENT\n", ERR_FD);
	return (NULL);
}

static t_ft_otool_sect	*get_text_sect_offset(t_ft_nm_hdrinfo *hdrinfo)
{
	struct load_command	cmd;
	uint32_t			load_command_size;
	uint32_t			load_command_offset;
	int					idx;
	t_ft_otool_sect		*text_sect;

	text_sect = NULL;
	slseek(hdrinfo->file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	while ((idx = goto_load_command(hdrinfo->file, hdrinfo, (int [3]){LC_SEGMENT, LC_SEGMENT_64}, &cmd)) != -1)
	{
		load_command_size = cmd.cmdsize;
		load_command_offset = hdrinfo->file->seek_ptr - hdrinfo->file->content - sizeof(struct load_command);
		slseek(hdrinfo->file, - ((int)sizeof(struct load_command)), SLSEEK_CUR);
		if ((hdrinfo->is_64) && (text_sect = search_sect(hdrinfo, hdrinfo->file->seek_ptr, SECT_TEXT, SEG_TEXT)))
		{
			text_sect->hdr = hdrinfo;
			break ;
		}
		slseek(hdrinfo->file, load_command_offset + load_command_size, SLSEEK_SET);
	}
	return (text_sect);
}

static int				dump_text_section(t_ft_nm_hdrinfo *hdrinfo, t_ft_otool_sect *text_section)
{
	uint32_t			n;
	uint32_t			byte_n;
	char				byte_val;

	n = 0;
	ft_printf("%s:\nContents of (__TEXT,__text) section - size: %u\n", hdrinfo->file->filepath, text_section->size);
	while (n < text_section->size)
	{
		ft_printf("%016" PRIx64 "%7c", text_section->address, ' ');
		byte_n = 0;
		while (byte_n < 16 && n + byte_n < text_section->size)
		{
			if (read_byte_range_at_pos(hdrinfo, &byte_val, sizeof(char), text_section->offset + n + byte_n))
				return (1);
			ft_printf(" %02" PRIx8 "", byte_val);
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
	t_ft_nm_sym			*symlist;

	if (init_header_info(file, &hdrs) == 1)
		return (1);
	if (!(hdr_to_use = goto_hdr_cpu_type(&hdrs, HOST_CPU_TYPE)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_X86)) && \
		!(hdr_to_use = goto_hdr_cpu_type(&hdrs, CPU_TYPE_I386)))
		return (1);
	if (check_load_commands(file, hdr_to_use) || \
		!(text_section = get_text_sect_offset(hdr_to_use)))
		return (1);
	if (slseek(file, hdr_to_use->fat_offset + text_section->offset, SLSEEK_SET) == -1)
		return (1);
	dump_text_section(hdr_to_use, text_section);
	free(text_section);
	return (0);
}
