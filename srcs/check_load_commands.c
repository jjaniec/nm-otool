/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_load_commands.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/13 20:29:00 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 18:51:42 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		check_symtab_command(t_ft_nm_hdrinfo *hdrinfo, \
					struct symtab_command *symtab)
{
	uint32_t		symsize;

	symsize = (symtab->nsyms * ((hdrinfo->is_64) ? \
			(sizeof(struct nlist_64)) : (sizeof(struct nlist))));
	if (!(symsize <= (symtab->stroff - symtab->symoff)))
	{
		ft_putstr_fd(hdrinfo->file->filepath, 2);
		ft_putstr_fd(" truncated or malformed object " \
			"(string table overlaps symbol table)\n", 2);
		return (1);
	}
	return (0);
}

static int		check_load_command_validity(t_ft_nm_hdrinfo *hdrinfo, \
					uint32_t *parsed_sizeofcmds, uint32_t *parsed_filesize)
{
	uint32_t			cmd;
	uint32_t			cmdsize;

	read_byte_range_at_pos(hdrinfo, &cmd, sizeof(uint32_t), \
		hdrinfo->fat_offset + hdrinfo->machhdr_size + \
		*parsed_sizeofcmds + \
		__offsetof(struct load_command, cmd));
	read_byte_range_at_pos(hdrinfo, &cmdsize, sizeof(uint32_t), \
		hdrinfo->fat_offset + hdrinfo->machhdr_size + \
		*parsed_sizeofcmds + \
		__offsetof(struct load_command, cmdsize));
	slseek(hdrinfo->file, hdrinfo->fat_offset + hdrinfo->machhdr_size + \
		*parsed_sizeofcmds, SLSEEK_SET);
	*parsed_sizeofcmds = *parsed_sizeofcmds + cmdsize;
	if ((cmd == LC_SEGMENT || cmd == LC_SEGMENT_64))
	{
		if (check_segment_command(hdrinfo, (void *)hdrinfo->file->seek_ptr, \
			parsed_filesize))
			return (1);
	}
	else if (cmd == LC_SYMTAB && \
		check_symtab_command(hdrinfo, (void *)hdrinfo->file->seek_ptr))
		return (1);
	return (0);
}

static int		check_load_commands_64(t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			parsed_ncmds;
	uint32_t			parsed_sizeofcmds;
	uint32_t			parsed_filesize;

	parsed_ncmds = 0;
	parsed_sizeofcmds = 0;
	parsed_filesize = 0;
	while (parsed_ncmds < hdrinfo->ncmds && \
			parsed_sizeofcmds < hdrinfo->sizeofcmds)
	{
		if (check_load_command_validity(hdrinfo, &parsed_sizeofcmds, \
			&parsed_filesize))
			return (2);
		parsed_ncmds++;
	}
	if (!(parsed_filesize <= hdrinfo->fat_size))
	{
		ft_putstr_fd("Inconsistent file offsets / " \
			"sizes in segment commands\n", 2);
		return (2);
	}
	return (!(parsed_ncmds == hdrinfo->ncmds && \
			parsed_sizeofcmds <= hdrinfo->sizeofcmds));
}

int				check_load_commands(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo)
{
	int		r;

	slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	if ((r = check_load_commands_64(hdrinfo)) == 1)
		ft_putstr_fd("Invalid load commands size / number\n", ERR_FD);
	return (r);
}
