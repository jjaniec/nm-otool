/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 22:29:47 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

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

	// printf("Filesize: %zu\n", file->totsiz);
	init_header_info(file, &hdrinfo);
	slseek(file, hdrinfo.offset + hdrinfo.machhdr_size, SLSEEK_SET);
	// if ((idx = goto_load_command(file, &hdrinfo, (int [2]){LC_SEGMENT, LC_SEGMENT_64}, &cmd)) != -1)
		// dump_sections(file, fileinfo, )
	if ((idx = goto_load_command(file, &hdrinfo, (int [/*2*/3]){LC_SYMTAB, 0}, &cmd)) != -1)
	{
		symlist = build_symbol_list(\
			file, &hdrinfo, \
			(void *)file->seek_ptr - sizeof(struct load_command));
		dump_symlist(symlist);
	}
	// dump_cmds(file, &hdrinfo);
	return (0);
}
