/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_header_info.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:27:45 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 18:32:31 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static uint32_t	get_ncmds(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo)
{
	struct mach_header		hdr;
	struct mach_header_64	hdr64;

	slseek(file, 0, SLSEEK_SET);
	if (fileinfo->is_64)
	{
		sseek_read(file, &hdr64, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr64, fileinfo->machhdr_size);
		// printf("Mach header 64:\nmagic\t\tcputype\t\tcpusubtype\tfiletype\tncmds\t\tsizeofcmds\tflags\n%x\t%u\t%u\t%u\t\t%u\t\t%u\t\t%x\n", hdr64.magic, hdr64.cputype, hdr64.cpusubtype, hdr64.filetype, hdr64.ncmds, hdr64.sizeofcmds, hdr64.flags);
		return (hdr64.ncmds);
	}
	else
	{
		sseek_read(file, &hdr, fileinfo->machhdr_size);
		if (!fileinfo->is_be)
			swap_byte_range(&hdr, fileinfo->machhdr_size);
		return (hdr.ncmds);
	}
	return (0);
}

int			init_header_info(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			magic;

	sseek_read(file, &magic, sizeof(uint32_t));
	hdrinfo->is_64 = is_magic_64(magic);
	hdrinfo->is_be = is_big_endian(magic);
	hdrinfo->magic = magic;
	hdrinfo->text_nsect = 1;
	hdrinfo->data_nsect = 2;
	hdrinfo->bss_nsect = NO_SECT;
	hdrinfo->machhdr_size = (hdrinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	if ((hdrinfo->ncmds = get_ncmds(file, hdrinfo)) == 0)
		return (-1);
	return (0);
}
