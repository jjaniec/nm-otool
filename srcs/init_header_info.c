/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_header_info.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:27:45 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/07 17:54:13 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static uint32_t		get_ncmds(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo)
{
	struct mach_header		hdr;
	struct mach_header_64	hdr64;

	slseek(file, hdrinfo->offset, SLSEEK_SET);
	if (hdrinfo->is_64)
	{
		sseek_read(file, &hdr64, hdrinfo->machhdr_size);
		if (!hdrinfo->is_be)
			swap_byte_range(&hdr64, hdrinfo->machhdr_size);
		// printf("Mach header 64:\nmagic\t\tcputype\t\tcpusubtype\tfiletype\tncmds\t\tsizeofcmds\tflags\n%x\t%u\t%u\t%u\t\t%u\t\t%u\t\t%x\n", hdr64.magic, hdr64.cputype, hdr64.cpusubtype, hdr64.filetype, hdr64.ncmds, hdr64.sizeofcmds, hdr64.flags);
		return (hdr64.ncmds);
	}
	else
	{
		sseek_read(file, &hdr, hdrinfo->machhdr_size);
		if (!hdrinfo->is_be)
			swap_byte_range(&hdr, hdrinfo->machhdr_size);
		return (hdr.ncmds);
	}
	return (0);
}

static t_ft_nm_hdrinfo	*init_macho_header(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, off_t offset)
{
	uint32_t			magic;

	slseek(file, offset, SLSEEK_SET);
	sseek_read(file, &magic, sizeof(uint32_t));
	hdrinfo->is_64 = is_magic_64(magic);
	hdrinfo->is_be = is_big_endian(magic);
	hdrinfo->magic = magic;
	hdrinfo->offset = offset;
	hdrinfo->text_nsect = 1;
	hdrinfo->data_nsect = 2;
	hdrinfo->bss_nsect = NO_SECT;
	hdrinfo->machhdr_size = (hdrinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	if ((hdrinfo->ncmds = get_ncmds(file, hdrinfo)) == 0)
		return (NULL);
	dprintf(2, "Init new header: magic: %x, is_be: %d, is_64: %d, offset %u, size: %zu, ncdms: %u\n", \
		hdrinfo->magic, hdrinfo->is_be, hdrinfo->is_64, hdrinfo->offset, hdrinfo->machhdr_size, hdrinfo->ncmds);
	return (hdrinfo);
}

static int			handle_fat_header(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, uint32_t nfat_arch, uint32_t magic)
{
	struct fat_arch		arch;
	uint32_t			fat_header_idx;
	t_ft_nm_hdrinfo		*cur_mach_header;
	t_ft_nm_hdrinfo		*new_mach_header;
	uint32_t			machhdr_offset;

	fat_header_idx = 0;
	cur_mach_header = NULL;
	while (fat_header_idx < nfat_arch)
	{
		slseek(file, sizeof(struct fat_header) + (sizeof(struct fat_arch) * fat_header_idx), SLSEEK_SET);
		sseek_read(file, &arch, sizeof(struct fat_arch));
		machhdr_offset = is_big_endian(magic) ? (magic) : swap_32bit(arch.offset);
		dprintf(2, "fat_header_idx: %d / %" PRIu32 " - cur_mach_header: %p, offset: %p\n", fat_header_idx, nfat_arch, cur_mach_header, machhdr_offset);
		if (cur_mach_header)
		{
			new_mach_header = malloc(sizeof(t_ft_nm_hdrinfo));
			init_macho_header(file, new_mach_header, machhdr_offset);
			cur_mach_header->next = new_mach_header;
			cur_mach_header = new_mach_header;
		}
		else
			cur_mach_header = init_macho_header(file, hdrinfo, machhdr_offset);
		fat_header_idx++;
	}
	return (0);
}

int					init_header_info(t_ft_nm_file *file, \
						t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			magic;
	uint32_t			nfat_arch;

	sseek_read(file, &magic, sizeof(uint32_t));
	if (is_magic_fat(magic))
	{
		sseek_read(file, &nfat_arch, sizeof(uint32_t));
		if (!is_big_endian(magic))
			nfat_arch = swap_32bit(nfat_arch);
		handle_fat_header(file, hdrinfo, nfat_arch, magic);
	}
	else if (is_magic_mach(magic))
		init_macho_header(file, hdrinfo, 0);
	else
		return (1);
	return (0);
}
