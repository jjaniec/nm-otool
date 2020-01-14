/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_read_symname.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/14 19:05:46 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 19:06:00 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

char			*safe_read_symname(t_ft_nm_hdrinfo *hdrinfo, \
						unsigned int index, bool handle_corrupt_syms, \
						struct symtab_command *symtabcmd)
{
	size_t		original_offset;
	uint32_t	strsize;
	char		*strtab;

	original_offset = hdrinfo->file->seek_ptr - hdrinfo->file->content;
	strtab = hdrinfo->fat_offset + symtabcmd->stroff + \
		(char *)hdrinfo->file->content;
	strsize = symtabcmd->strsize;
	if (index > strsize && !handle_corrupt_syms)
	{
		ft_putstr_fd(hdrinfo->file->filepath, 2);
		ft_putstr_fd(": truncated or malformed object " \
			"(bad string table index past the end " \
			"of string table for symbol)", 2);
		return (NULL);
	}
	if (slseek(hdrinfo->file, (int)(&strtab[index] - \
		hdrinfo->file->content), SLSEEK_SET) == -1)
		return (NULL);
	return ((&strtab[index]));
}
