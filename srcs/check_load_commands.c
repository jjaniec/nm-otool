/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_load_commands.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/13 20:29:00 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/14 18:28:09 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		check_load_commands_64(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			cmdsize;
	uint32_t			parsed_ncmds;
	uint32_t			parsed_sizeofcmds;
	uint32_t			seek_ptr_save;

	parsed_ncmds = 0;
	parsed_sizeofcmds = 0;
	while (parsed_ncmds < hdrinfo->ncmds && \
			parsed_sizeofcmds < hdrinfo->sizeofcmds)
	{
		read_byte_range_at_pos(hdrinfo, &cmdsize, sizeof(uint32_t), \
			hdrinfo->fat_offset + hdrinfo->machhdr_size + \
			parsed_sizeofcmds + \
			__offsetof(struct load_command, cmdsize));
		parsed_sizeofcmds += cmdsize;
		parsed_ncmds++;
	}
	dprintf(DEBUG_FD, "Parsed ncmds: %u / %u - parsed_size: %u / %u\n", parsed_ncmds, hdrinfo->ncmds, parsed_sizeofcmds, hdrinfo->sizeofcmds);
	return (!(parsed_ncmds == hdrinfo->ncmds && \
			parsed_sizeofcmds == hdrinfo->sizeofcmds));
}

int			check_load_commands(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo)
{
	int		r;

	slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	if ((r = check_load_commands_64(file, hdrinfo)))
		dprintf(ERR_FD, ERR_PREFIX "Invalid load commands size / number\n");
	return (r);
}
