/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_segment_command.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/13 22:19:11 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/13 22:19:27 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		check_segment_command_64(t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command_64 *seg, uint32_t *parsed_filesize)
{
	if ((seg->fileoff - *parsed_filesize) % 4096 && \
		!(seg->fileoff == (hdrinfo->machhdr_size + hdrinfo->sizeofcmds)))
	{
		ft_putstr_fd("Inconsistent file offsets / " \
			"sizes in segment_command_64\n", 2);
		return (1);
	}
	if (seg->cmdsize < (sizeof(struct segment_command_64) + seg->nsects * \
		sizeof(struct section_64)))
	{
		ft_putstr_fd("Inconsistent cmdsize in LC_SEGMENT_64 " \
			"for the number of sections\n", 2);
		return (1);
	}
	*parsed_filesize = *parsed_filesize + seg->filesize;
	return (0);
}

static int		check_segment_command_32(t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command *seg, uint32_t *parsed_filesize)
{
	if ((seg->fileoff - *parsed_filesize) % 4096 && \
		!(seg->fileoff == (hdrinfo->machhdr_size + hdrinfo->sizeofcmds)))
	{
		ft_putstr_fd("Inconsistent file offsets / " \
			"sizes in segment_command_32\n", 2);
		return (1);
	}
	if (seg->cmdsize < (sizeof(struct segment_command) + seg->nsects \
		* sizeof(struct section)))
	{
		ft_putstr_fd("Inconsistent cmdsize in LC_SEGMENT " \
			"for the number of sections\n", 2);
		return (1);
	}
	*parsed_filesize = *parsed_filesize + seg->filesize;
	return (0);
}

int				check_segment_command(t_ft_nm_hdrinfo *hdrinfo, \
					void *seg, uint32_t *parsed_filesize)
{
	if (hdrinfo->is_64)
		return (check_segment_command_64(hdrinfo, seg, parsed_filesize));
	return (check_segment_command_32(hdrinfo, seg, parsed_filesize));
}
