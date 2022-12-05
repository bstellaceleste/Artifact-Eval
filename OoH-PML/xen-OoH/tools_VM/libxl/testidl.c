
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libxl.h"
#include "libxl_utils.h"

static int test_rand(unsigned max)
{
    /* We are not using rand() for its cryptographic properies. */
    return rand() % max;
}

static char *rand_str(void)
{
    int i, sz = test_rand(32);
    char *s = malloc(sz+1);
    assert(s);
    for (i=0; i<sz; i++)
        s[i] = 'a' + test_rand(26);
    s[i] = '\0';
    return s;
}

static void rand_bytes(uint8_t *p, size_t sz)
{
    int i;
    for (i=0; i<sz; i++)
        p[i] = test_rand(256);
}

static void libxl_bitmap_rand_init(libxl_bitmap *bitmap)
{
    int i;
    bitmap->size = test_rand(16);
    bitmap->map = calloc(bitmap->size, sizeof(*bitmap->map));
    assert(bitmap->map);
    libxl_for_each_bit(i, *bitmap) {
        if (test_rand(2))
            libxl_bitmap_set(bitmap, i);
        else
            libxl_bitmap_reset(bitmap, i);
    }
}

static void libxl_key_value_list_rand_init(libxl_key_value_list *pkvl)
{
    int i, nr_kvp = test_rand(16);
    libxl_key_value_list kvl = calloc(nr_kvp+1, 2*sizeof(char *));
    assert(kvl);

    for (i = 0; i<2*nr_kvp; i += 2) {
        kvl[i] = rand_str();
        if (test_rand(8))
            kvl[i+1] = rand_str();
        else
            kvl[i+1] = NULL;
    }
    kvl[i] = NULL;
    kvl[i+1] = NULL;
    *pkvl = kvl;
}

static void libxl_cpuid_policy_list_rand_init(libxl_cpuid_policy_list *pp)
{
    int i, nr_policies = test_rand(16);
    struct {
        const char *n;
        int w;
    } options[] = {
      /* A random selection from libxl_cpuid_parse_config */
        {"maxleaf",     32},
        {"family",       8},
        {"model",        8},
        {"stepping",     4},
        {"localapicid",  8},
        {"proccount",    8},
        {"clflush",      8},
        {"brandid",      8},
        {"f16c",         1},
        {"avx",          1},
        {"osxsave",      1},
        {"xsave",        1},
        {"aes",          1},
        {"popcnt",       1},
        {"movbe",        1},
        {"x2apic",       1},
        {"sse4.2",       1},
        {"sse4.1",       1},
        {"dca",          1},
        {"pdcm",         1},
        {"procpkg",      6},
    };
    const int nr_options = sizeof(options)/sizeof(options[0]);
    char buf[64];
    libxl_cpuid_policy_list p = NULL;

    for (i = 0; i < nr_policies; i++) {
        int opt = test_rand(nr_options);
        int val = test_rand(1<<options[opt].w);
        snprintf(buf, 64, "%s=%#x", options[opt].n, val);
        libxl_cpuid_parse_config(&p, buf);
    }
    *pp = p;
}

static void libxl_string_list_rand_init(libxl_string_list *p)
{
    int i, nr = test_rand(16);
    libxl_string_list l = calloc(nr+1, sizeof(char *));
    assert(l);

    for (i = 0; i<nr; i++) {
        l[i] = rand_str();
    }
    l[i] = NULL;
    *p = l;
}
static void libxl_defbool_rand_init(libxl_defbool *p);
static void libxl_defbool_rand_init(libxl_defbool *p)
{
    libxl_defbool_set(p, test_rand(2));
}

static void libxl_domid_rand_init(libxl_domid *p);
static void libxl_domid_rand_init(libxl_domid *p)
{
    *p = test_rand(sizeof(*p) * 8);
}

static void libxl_devid_rand_init(libxl_devid *p);
static void libxl_devid_rand_init(libxl_devid *p)
{
    *p = test_rand(sizeof(*p) * 8);
}

static void libxl_uuid_rand_init(libxl_uuid *p);
static void libxl_uuid_rand_init(libxl_uuid *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_mac_rand_init(libxl_mac *p);
static void libxl_mac_rand_init(libxl_mac *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_hwcap_rand_init(libxl_hwcap *p);
static void libxl_hwcap_rand_init(libxl_hwcap *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_ms_vm_genid_rand_init(libxl_ms_vm_genid *p);
static void libxl_ms_vm_genid_rand_init(libxl_ms_vm_genid *p)
{
    rand_bytes((uint8_t *)p, sizeof(*p));
}

static void libxl_ev_link_rand_init(libxl_ev_link *p);
static void libxl_ev_link_rand_init(libxl_ev_link *p)
{
}

static void libxl_domain_unpriv_rand_init(libxl_domain_unpriv *p);
static void libxl_domain_unpriv_rand_init(libxl_domain_unpriv *p)
{
    *p = LIBXL_DOMAIN_UNPRIV_DOM0;
}

static void libxl_error_rand_init(libxl_error *p);
static void libxl_error_rand_init(libxl_error *p)
{
    *p = ERROR_NOPARAVIRT;
}

static void libxl_domain_type_rand_init(libxl_domain_type *p);
static void libxl_domain_type_rand_init(libxl_domain_type *p)
{
    *p = LIBXL_DOMAIN_TYPE_PV;
}

static void libxl_rdm_reserve_strategy_rand_init(libxl_rdm_reserve_strategy *p);
static void libxl_rdm_reserve_strategy_rand_init(libxl_rdm_reserve_strategy *p)
{
    *p = LIBXL_RDM_RESERVE_STRATEGY_HOST;
}

static void libxl_rdm_reserve_policy_rand_init(libxl_rdm_reserve_policy *p);
static void libxl_rdm_reserve_policy_rand_init(libxl_rdm_reserve_policy *p)
{
    *p = LIBXL_RDM_RESERVE_POLICY_INVALID;
}

static void libxl_channel_connection_rand_init(libxl_channel_connection *p);
static void libxl_channel_connection_rand_init(libxl_channel_connection *p)
{
    *p = LIBXL_CHANNEL_CONNECTION_PTY;
}

static void libxl_device_model_version_rand_init(libxl_device_model_version *p);
static void libxl_device_model_version_rand_init(libxl_device_model_version *p)
{
    *p = LIBXL_DEVICE_MODEL_VERSION_UNKNOWN;
}

static void libxl_console_type_rand_init(libxl_console_type *p);
static void libxl_console_type_rand_init(libxl_console_type *p)
{
    *p = LIBXL_CONSOLE_TYPE_VUART;
}

static void libxl_disk_format_rand_init(libxl_disk_format *p);
static void libxl_disk_format_rand_init(libxl_disk_format *p)
{
    *p = LIBXL_DISK_FORMAT_EMPTY;
}

static void libxl_disk_backend_rand_init(libxl_disk_backend *p);
static void libxl_disk_backend_rand_init(libxl_disk_backend *p)
{
    *p = LIBXL_DISK_BACKEND_QDISK;
}

static void libxl_nic_type_rand_init(libxl_nic_type *p);
static void libxl_nic_type_rand_init(libxl_nic_type *p)
{
    *p = LIBXL_NIC_TYPE_VIF_IOEMU;
}

static void libxl_action_on_shutdown_rand_init(libxl_action_on_shutdown *p);
static void libxl_action_on_shutdown_rand_init(libxl_action_on_shutdown *p)
{
    *p = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART;
}

static void libxl_trigger_rand_init(libxl_trigger *p);
static void libxl_trigger_rand_init(libxl_trigger *p)
{
    *p = LIBXL_TRIGGER_UNKNOWN;
}

static void libxl_tsc_mode_rand_init(libxl_tsc_mode *p);
static void libxl_tsc_mode_rand_init(libxl_tsc_mode *p)
{
    *p = LIBXL_TSC_MODE_NATIVE;
}

static void libxl_gfx_passthru_kind_rand_init(libxl_gfx_passthru_kind *p);
static void libxl_gfx_passthru_kind_rand_init(libxl_gfx_passthru_kind *p)
{
    *p = LIBXL_GFX_PASSTHRU_KIND_IGD;
}

static void libxl_timer_mode_rand_init(libxl_timer_mode *p);
static void libxl_timer_mode_rand_init(libxl_timer_mode *p)
{
    *p = LIBXL_TIMER_MODE_UNKNOWN;
}

static void libxl_bios_type_rand_init(libxl_bios_type *p);
static void libxl_bios_type_rand_init(libxl_bios_type *p)
{
    *p = LIBXL_BIOS_TYPE_UNKNOWN;
}

static void libxl_scheduler_rand_init(libxl_scheduler *p);
static void libxl_scheduler_rand_init(libxl_scheduler *p)
{
    *p = LIBXL_SCHEDULER_NULL;
}

static void libxl_shutdown_reason_rand_init(libxl_shutdown_reason *p);
static void libxl_shutdown_reason_rand_init(libxl_shutdown_reason *p)
{
    *p = LIBXL_SHUTDOWN_REASON_UNKNOWN;
}

static void libxl_vga_interface_type_rand_init(libxl_vga_interface_type *p);
static void libxl_vga_interface_type_rand_init(libxl_vga_interface_type *p)
{
    *p = LIBXL_VGA_INTERFACE_TYPE_STD;
}

static void libxl_vendor_device_rand_init(libxl_vendor_device *p);
static void libxl_vendor_device_rand_init(libxl_vendor_device *p)
{
    *p = LIBXL_VENDOR_DEVICE_NONE;
}

static void libxl_viridian_enlightenment_rand_init(libxl_viridian_enlightenment *p);
static void libxl_viridian_enlightenment_rand_init(libxl_viridian_enlightenment *p)
{
    *p = LIBXL_VIRIDIAN_ENLIGHTENMENT_CRASH_CTL;
}

static void libxl_hdtype_rand_init(libxl_hdtype *p);
static void libxl_hdtype_rand_init(libxl_hdtype *p)
{
    *p = LIBXL_HDTYPE_IDE;
}

static void libxl_checkpointed_stream_rand_init(libxl_checkpointed_stream *p);
static void libxl_checkpointed_stream_rand_init(libxl_checkpointed_stream *p)
{
    *p = LIBXL_CHECKPOINTED_STREAM_NONE;
}

static void libxl_vuart_type_rand_init(libxl_vuart_type *p);
static void libxl_vuart_type_rand_init(libxl_vuart_type *p)
{
    *p = LIBXL_VUART_TYPE_SBSA_UART;
}

static void libxl_ioport_range_rand_init(libxl_ioport_range *p);
static void libxl_ioport_range_rand_init(libxl_ioport_range *p)
{
    p->first = test_rand(sizeof(p->first) * 8);
    p->number = test_rand(sizeof(p->number) * 8);
}

static void libxl_iomem_range_rand_init(libxl_iomem_range *p);
static void libxl_iomem_range_rand_init(libxl_iomem_range *p)
{
    p->start = test_rand(sizeof(p->start) * 8);
    p->number = test_rand(sizeof(p->number) * 8);
    p->gfn = test_rand(sizeof(p->gfn) * 8);
}

static void libxl_vga_interface_info_rand_init(libxl_vga_interface_info *p);
static void libxl_vga_interface_info_rand_init(libxl_vga_interface_info *p)
{
    p->kind = LIBXL_VGA_INTERFACE_TYPE_NONE;
}

static void libxl_vnc_info_rand_init(libxl_vnc_info *p);
static void libxl_vnc_info_rand_init(libxl_vnc_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    p->listen = rand_str();
    p->passwd = rand_str();
    p->display = test_rand(sizeof(p->display) * 8);
    libxl_defbool_rand_init(&p->findunused);
}

static void libxl_spice_info_rand_init(libxl_spice_info *p);
static void libxl_spice_info_rand_init(libxl_spice_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    p->port = test_rand(sizeof(p->port) * 8);
    p->tls_port = test_rand(sizeof(p->tls_port) * 8);
    p->host = rand_str();
    libxl_defbool_rand_init(&p->disable_ticketing);
    p->passwd = rand_str();
    libxl_defbool_rand_init(&p->agent_mouse);
    libxl_defbool_rand_init(&p->vdagent);
    libxl_defbool_rand_init(&p->clipboard_sharing);
    p->usbredirection = test_rand(sizeof(p->usbredirection) * 8);
    p->image_compression = rand_str();
    p->streaming_video = rand_str();
}

static void libxl_sdl_info_rand_init(libxl_sdl_info *p);
static void libxl_sdl_info_rand_init(libxl_sdl_info *p)
{
    libxl_defbool_rand_init(&p->enable);
    libxl_defbool_rand_init(&p->opengl);
    p->display = rand_str();
    p->xauthority = rand_str();
}

static void libxl_dominfo_rand_init(libxl_dominfo *p);
static void libxl_dominfo_rand_init(libxl_dominfo *p)
{
    libxl_uuid_rand_init(&p->uuid);
    libxl_domid_rand_init(&p->domid);
    p->ssidref = test_rand(sizeof(p->ssidref) * 8);
    p->ssid_label = rand_str();
    p->running = test_rand(2);
    p->blocked = test_rand(2);
    p->paused = test_rand(2);
    p->shutdown = test_rand(2);
    p->dying = test_rand(2);
    p->never_stop = test_rand(2);
    p->shutdown_reason = LIBXL_SHUTDOWN_REASON_WATCHDOG;
    p->outstanding_memkb = test_rand(sizeof(p->outstanding_memkb) * 8);
    p->current_memkb = test_rand(sizeof(p->current_memkb) * 8);
    p->shared_memkb = test_rand(sizeof(p->shared_memkb) * 8);
    p->paged_memkb = test_rand(sizeof(p->paged_memkb) * 8);
    p->max_memkb = test_rand(sizeof(p->max_memkb) * 8);
    p->cpu_time = test_rand(sizeof(p->cpu_time) * 8);
    p->vcpu_max_id = test_rand(sizeof(p->vcpu_max_id) * 8);
    p->vcpu_online = test_rand(sizeof(p->vcpu_online) * 8);
    p->cpupool = test_rand(sizeof(p->cpupool) * 8);
    p->domain_type = LIBXL_DOMAIN_TYPE_PVH;
}

static void libxl_cpupoolinfo_rand_init(libxl_cpupoolinfo *p);
static void libxl_cpupoolinfo_rand_init(libxl_cpupoolinfo *p)
{
    p->poolid = test_rand(sizeof(p->poolid) * 8);
    p->pool_name = rand_str();
    p->sched = LIBXL_SCHEDULER_CREDIT2;
    p->n_dom = test_rand(sizeof(p->n_dom) * 8);
    libxl_bitmap_rand_init(&p->cpumap);
}

static void libxl_channelinfo_rand_init(libxl_channelinfo *p);
static void libxl_channelinfo_rand_init(libxl_channelinfo *p)
{
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    libxl_devid_rand_init(&p->devid);
    p->state = test_rand(sizeof(p->state) * 8);
    p->evtch = test_rand(sizeof(p->evtch) * 8);
    p->rref = test_rand(sizeof(p->rref) * 8);
    p->connection = LIBXL_CHANNEL_CONNECTION_PTY;
    switch (p->connection) {
    case LIBXL_CHANNEL_CONNECTION_UNKNOWN:
        break;
    case LIBXL_CHANNEL_CONNECTION_PTY:
        p->u.pty.path = rand_str();
        break;
    case LIBXL_CHANNEL_CONNECTION_SOCKET:
        break;
    }
}

static void libxl_vminfo_rand_init(libxl_vminfo *p);
static void libxl_vminfo_rand_init(libxl_vminfo *p)
{
    libxl_uuid_rand_init(&p->uuid);
    libxl_domid_rand_init(&p->domid);
}

static void libxl_version_info_rand_init(libxl_version_info *p);
static void libxl_version_info_rand_init(libxl_version_info *p)
{
    p->xen_version_major = test_rand(sizeof(p->xen_version_major) * 8);
    p->xen_version_minor = test_rand(sizeof(p->xen_version_minor) * 8);
    p->xen_version_extra = rand_str();
    p->compiler = rand_str();
    p->compile_by = rand_str();
    p->compile_domain = rand_str();
    p->compile_date = rand_str();
    p->capabilities = rand_str();
    p->changeset = rand_str();
    p->virt_start = test_rand(sizeof(p->virt_start) * 8);
    p->pagesize = test_rand(sizeof(p->pagesize) * 8);
    p->commandline = rand_str();
    p->build_id = rand_str();
}

static void libxl_domain_create_info_rand_init(libxl_domain_create_info *p);
static void libxl_domain_create_info_rand_init(libxl_domain_create_info *p)
{
    p->type = LIBXL_DOMAIN_TYPE_PVH;
    libxl_defbool_rand_init(&p->hap);
    libxl_defbool_rand_init(&p->oos);
    p->ssidref = test_rand(sizeof(p->ssidref) * 8);
    p->ssid_label = rand_str();
    p->name = rand_str();
    libxl_uuid_rand_init(&p->uuid);
    libxl_key_value_list_rand_init(&p->xsdata);
    libxl_key_value_list_rand_init(&p->platformdata);
    p->poolid = test_rand(sizeof(p->poolid) * 8);
    p->pool_name = rand_str();
    libxl_defbool_rand_init(&p->run_hotplug_scripts);
    libxl_defbool_rand_init(&p->driver_domain);
}

static void libxl_domain_restore_params_rand_init(libxl_domain_restore_params *p);
static void libxl_domain_restore_params_rand_init(libxl_domain_restore_params *p)
{
    p->checkpointed_stream = test_rand(sizeof(p->checkpointed_stream) * 8);
    p->stream_version = test_rand(sizeof(p->stream_version) * 8);
    p->colo_proxy_script = rand_str();
    libxl_defbool_rand_init(&p->userspace_colo_proxy);
}

static void libxl_sched_params_rand_init(libxl_sched_params *p);
static void libxl_sched_params_rand_init(libxl_sched_params *p)
{
    p->vcpuid = test_rand(sizeof(p->vcpuid) * 8);
    p->weight = test_rand(sizeof(p->weight) * 8);
    p->cap = test_rand(sizeof(p->cap) * 8);
    p->period = test_rand(sizeof(p->period) * 8);
    p->extratime = test_rand(sizeof(p->extratime) * 8);
    p->budget = test_rand(sizeof(p->budget) * 8);
}

static void libxl_vcpu_sched_params_rand_init(libxl_vcpu_sched_params *p);
static void libxl_vcpu_sched_params_rand_init(libxl_vcpu_sched_params *p)
{
    p->sched = LIBXL_SCHEDULER_UNKNOWN;
    p->num_vcpus = test_rand(8);
    p->vcpus = calloc(p->num_vcpus, sizeof(*p->vcpus));
    assert(p->vcpus);
    {
        int i;
        for (i=0; i<p->num_vcpus; i++)
            libxl_sched_params_rand_init(&p->vcpus[i]);
    }
}

static void libxl_domain_sched_params_rand_init(libxl_domain_sched_params *p);
static void libxl_domain_sched_params_rand_init(libxl_domain_sched_params *p)
{
    p->sched = LIBXL_SCHEDULER_CREDIT2;
    p->weight = test_rand(sizeof(p->weight) * 8);
    p->cap = test_rand(sizeof(p->cap) * 8);
    p->period = test_rand(sizeof(p->period) * 8);
    p->budget = test_rand(sizeof(p->budget) * 8);
    p->extratime = test_rand(sizeof(p->extratime) * 8);
    p->slice = test_rand(sizeof(p->slice) * 8);
    p->latency = test_rand(sizeof(p->latency) * 8);
}

static void libxl_vnode_info_rand_init(libxl_vnode_info *p);
static void libxl_vnode_info_rand_init(libxl_vnode_info *p)
{
    p->memkb = test_rand(sizeof(p->memkb) * 8);
    p->num_distances = test_rand(8);
    p->distances = calloc(p->num_distances, sizeof(*p->distances));
    assert(p->distances);
    {
        int i;
        for (i=0; i<p->num_distances; i++)
            p->distances[i] = test_rand(sizeof(p->distances[i]) * 8);
    }
    p->pnode = test_rand(sizeof(p->pnode) * 8);
    libxl_bitmap_rand_init(&p->vcpus);
}

static void libxl_gic_version_rand_init(libxl_gic_version *p);
static void libxl_gic_version_rand_init(libxl_gic_version *p)
{
    *p = LIBXL_GIC_VERSION_V3;
}

static void libxl_rdm_reserve_rand_init(libxl_rdm_reserve *p);
static void libxl_rdm_reserve_rand_init(libxl_rdm_reserve *p)
{
    p->strategy = LIBXL_RDM_RESERVE_STRATEGY_IGNORE;
    p->policy = LIBXL_RDM_RESERVE_POLICY_STRICT;
}

static void libxl_altp2m_mode_rand_init(libxl_altp2m_mode *p);
static void libxl_altp2m_mode_rand_init(libxl_altp2m_mode *p)
{
    *p = LIBXL_ALTP2M_MODE_MIXED;
}

static void libxl_domain_build_info_rand_init(libxl_domain_build_info *p);
static void libxl_domain_build_info_rand_init(libxl_domain_build_info *p)
{
    p->max_vcpus = test_rand(sizeof(p->max_vcpus) * 8);
    libxl_bitmap_rand_init(&p->avail_vcpus);
    libxl_bitmap_rand_init(&p->cpumap);
    libxl_bitmap_rand_init(&p->nodemap);
    p->num_vcpu_hard_affinity = test_rand(8);
    p->vcpu_hard_affinity = calloc(p->num_vcpu_hard_affinity, sizeof(*p->vcpu_hard_affinity));
    assert(p->vcpu_hard_affinity);
    {
        int i;
        for (i=0; i<p->num_vcpu_hard_affinity; i++)
            libxl_bitmap_rand_init(&p->vcpu_hard_affinity[i]);
    }
    p->num_vcpu_soft_affinity = test_rand(8);
    p->vcpu_soft_affinity = calloc(p->num_vcpu_soft_affinity, sizeof(*p->vcpu_soft_affinity));
    assert(p->vcpu_soft_affinity);
    {
        int i;
        for (i=0; i<p->num_vcpu_soft_affinity; i++)
            libxl_bitmap_rand_init(&p->vcpu_soft_affinity[i]);
    }
    libxl_defbool_rand_init(&p->numa_placement);
    p->tsc_mode = LIBXL_TSC_MODE_NATIVE_PARAVIRT;
    p->max_memkb = test_rand(sizeof(p->max_memkb) * 8);
    p->target_memkb = test_rand(sizeof(p->target_memkb) * 8);
    p->video_memkb = test_rand(sizeof(p->video_memkb) * 8);
    p->shadow_memkb = test_rand(sizeof(p->shadow_memkb) * 8);
    p->rtc_timeoffset = test_rand(sizeof(p->rtc_timeoffset) * 8);
    p->exec_ssidref = test_rand(sizeof(p->exec_ssidref) * 8);
    p->exec_ssid_label = rand_str();
    libxl_defbool_rand_init(&p->localtime);
    libxl_defbool_rand_init(&p->disable_migrate);
    libxl_cpuid_policy_list_rand_init(&p->cpuid);
    p->blkdev_start = rand_str();
    p->num_vnuma_nodes = test_rand(8);
    p->vnuma_nodes = calloc(p->num_vnuma_nodes, sizeof(*p->vnuma_nodes));
    assert(p->vnuma_nodes);
    {
        int i;
        for (i=0; i<p->num_vnuma_nodes; i++)
            libxl_vnode_info_rand_init(&p->vnuma_nodes[i]);
    }
    p->max_grant_frames = test_rand(sizeof(p->max_grant_frames) * 8);
    p->max_maptrack_frames = test_rand(sizeof(p->max_maptrack_frames) * 8);
    p->device_model_version = LIBXL_DEVICE_MODEL_VERSION_UNKNOWN;
    libxl_defbool_rand_init(&p->device_model_stubdomain);
    p->device_model = rand_str();
    p->device_model_ssidref = test_rand(sizeof(p->device_model_ssidref) * 8);
    p->device_model_ssid_label = rand_str();
    p->device_model_user = rand_str();
    libxl_string_list_rand_init(&p->extra);
    libxl_string_list_rand_init(&p->extra_pv);
    libxl_string_list_rand_init(&p->extra_hvm);
    libxl_domain_sched_params_rand_init(&p->sched_params);
    p->num_ioports = test_rand(8);
    p->ioports = calloc(p->num_ioports, sizeof(*p->ioports));
    assert(p->ioports);
    {
        int i;
        for (i=0; i<p->num_ioports; i++)
            libxl_ioport_range_rand_init(&p->ioports[i]);
    }
    p->num_irqs = test_rand(8);
    p->irqs = calloc(p->num_irqs, sizeof(*p->irqs));
    assert(p->irqs);
    {
        int i;
        for (i=0; i<p->num_irqs; i++)
            p->irqs[i] = test_rand(sizeof(p->irqs[i]) * 8);
    }
    p->num_iomem = test_rand(8);
    p->iomem = calloc(p->num_iomem, sizeof(*p->iomem));
    assert(p->iomem);
    {
        int i;
        for (i=0; i<p->num_iomem; i++)
            libxl_iomem_range_rand_init(&p->iomem[i]);
    }
    libxl_defbool_rand_init(&p->claim_mode);
    p->event_channels = test_rand(sizeof(p->event_channels) * 8);
    p->kernel = rand_str();
    p->cmdline = rand_str();
    p->ramdisk = rand_str();
    p->device_tree = rand_str();
    libxl_defbool_rand_init(&p->acpi);
    p->bootloader = rand_str();
    libxl_string_list_rand_init(&p->bootloader_args);
    p->timer_mode = LIBXL_TIMER_MODE_UNKNOWN;
    libxl_defbool_rand_init(&p->nested_hvm);
    libxl_defbool_rand_init(&p->apic);
    libxl_defbool_rand_init(&p->dm_restrict);
    p->type = LIBXL_DOMAIN_TYPE_INVALID;
    switch (p->type) {
    case LIBXL_DOMAIN_TYPE_HVM:
        p->u.hvm.firmware = rand_str();
        p->u.hvm.bios = LIBXL_BIOS_TYPE_ROMBIOS;
        libxl_defbool_rand_init(&p->u.hvm.pae);
        libxl_defbool_rand_init(&p->u.hvm.apic);
        libxl_defbool_rand_init(&p->u.hvm.acpi);
        libxl_defbool_rand_init(&p->u.hvm.acpi_s3);
        libxl_defbool_rand_init(&p->u.hvm.acpi_s4);
        libxl_defbool_rand_init(&p->u.hvm.acpi_laptop_slate);
        libxl_defbool_rand_init(&p->u.hvm.nx);
        libxl_defbool_rand_init(&p->u.hvm.viridian);
        libxl_bitmap_rand_init(&p->u.hvm.viridian_enable);
        libxl_bitmap_rand_init(&p->u.hvm.viridian_disable);
        p->u.hvm.timeoffset = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.hpet);
        libxl_defbool_rand_init(&p->u.hvm.vpt_align);
        p->u.hvm.mmio_hole_memkb = test_rand(sizeof(p->u.hvm.mmio_hole_memkb) * 8);
        p->u.hvm.timer_mode = LIBXL_TIMER_MODE_UNKNOWN;
        libxl_defbool_rand_init(&p->u.hvm.nested_hvm);
        libxl_defbool_rand_init(&p->u.hvm.altp2m);
        p->u.hvm.system_firmware = rand_str();
        p->u.hvm.smbios_firmware = rand_str();
        p->u.hvm.acpi_firmware = rand_str();
        p->u.hvm.hdtype = LIBXL_HDTYPE_AHCI;
        libxl_defbool_rand_init(&p->u.hvm.nographic);
        libxl_vga_interface_info_rand_init(&p->u.hvm.vga);
        libxl_vnc_info_rand_init(&p->u.hvm.vnc);
        p->u.hvm.keymap = rand_str();
        libxl_sdl_info_rand_init(&p->u.hvm.sdl);
        libxl_spice_info_rand_init(&p->u.hvm.spice);
        libxl_defbool_rand_init(&p->u.hvm.gfx_passthru);
        p->u.hvm.gfx_passthru_kind = LIBXL_GFX_PASSTHRU_KIND_DEFAULT;
        p->u.hvm.serial = rand_str();
        p->u.hvm.boot = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.usb);
        p->u.hvm.usbversion = test_rand(sizeof(p->u.hvm.usbversion) * 8);
        p->u.hvm.usbdevice = rand_str();
        p->u.hvm.soundhw = rand_str();
        libxl_defbool_rand_init(&p->u.hvm.xen_platform_pci);
        libxl_string_list_rand_init(&p->u.hvm.usbdevice_list);
        p->u.hvm.vendor_device = LIBXL_VENDOR_DEVICE_NONE;
        libxl_ms_vm_genid_rand_init(&p->u.hvm.ms_vm_genid);
        libxl_string_list_rand_init(&p->u.hvm.serial_list);
        libxl_rdm_reserve_rand_init(&p->u.hvm.rdm);
        p->u.hvm.rdm_mem_boundary_memkb = test_rand(sizeof(p->u.hvm.rdm_mem_boundary_memkb) * 8);
        p->u.hvm.mca_caps = test_rand(sizeof(p->u.hvm.mca_caps) * 8);
        break;
    case LIBXL_DOMAIN_TYPE_PV:
        p->u.pv.kernel = rand_str();
        p->u.pv.slack_memkb = test_rand(sizeof(p->u.pv.slack_memkb) * 8);
        p->u.pv.bootloader = rand_str();
        libxl_string_list_rand_init(&p->u.pv.bootloader_args);
        p->u.pv.cmdline = rand_str();
        p->u.pv.ramdisk = rand_str();
        libxl_defbool_rand_init(&p->u.pv.e820_host);
        break;
    case LIBXL_DOMAIN_TYPE_PVH:
        break;
    case LIBXL_DOMAIN_TYPE_INVALID:
        break;
    }
    p->arch_arm.gic_version = LIBXL_GIC_VERSION_DEFAULT;
    p->arch_arm.vuart = LIBXL_VUART_TYPE_UNKNOWN;
    p->altp2m = LIBXL_ALTP2M_MODE_EXTERNAL;
}

static void libxl_device_vfb_rand_init(libxl_device_vfb *p);
static void libxl_device_vfb_rand_init(libxl_device_vfb *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    libxl_vnc_info_rand_init(&p->vnc);
    libxl_sdl_info_rand_init(&p->sdl);
    p->keymap = rand_str();
}

static void libxl_device_vkb_rand_init(libxl_device_vkb *p);
static void libxl_device_vkb_rand_init(libxl_device_vkb *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
}

static void libxl_device_disk_rand_init(libxl_device_disk *p);
static void libxl_device_disk_rand_init(libxl_device_disk *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    p->pdev_path = rand_str();
    p->vdev = rand_str();
    p->backend = LIBXL_DISK_BACKEND_QDISK;
    p->format = LIBXL_DISK_FORMAT_RAW;
    p->script = rand_str();
    p->removable = test_rand(sizeof(p->removable) * 8);
    p->readwrite = test_rand(sizeof(p->readwrite) * 8);
    p->is_cdrom = test_rand(sizeof(p->is_cdrom) * 8);
    p->direct_io_safe = test_rand(2);
    libxl_defbool_rand_init(&p->discard_enable);
    libxl_defbool_rand_init(&p->colo_enable);
    libxl_defbool_rand_init(&p->colo_restore_enable);
    p->colo_host = rand_str();
    p->colo_port = test_rand(sizeof(p->colo_port) * 8);
    p->colo_export = rand_str();
    p->active_disk = rand_str();
    p->hidden_disk = rand_str();
}

static void libxl_device_nic_rand_init(libxl_device_nic *p);
static void libxl_device_nic_rand_init(libxl_device_nic *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    p->mtu = test_rand(sizeof(p->mtu) * 8);
    p->model = rand_str();
    libxl_mac_rand_init(&p->mac);
    p->ip = rand_str();
    p->bridge = rand_str();
    p->ifname = rand_str();
    p->script = rand_str();
    p->nictype = LIBXL_NIC_TYPE_UNKNOWN;
    p->rate_bytes_per_interval = test_rand(sizeof(p->rate_bytes_per_interval) * 8);
    p->rate_interval_usecs = test_rand(sizeof(p->rate_interval_usecs) * 8);
    p->gatewaydev = rand_str();
    p->coloft_forwarddev = rand_str();
    p->colo_sock_mirror_id = rand_str();
    p->colo_sock_mirror_ip = rand_str();
    p->colo_sock_mirror_port = rand_str();
    p->colo_sock_compare_pri_in_id = rand_str();
    p->colo_sock_compare_pri_in_ip = rand_str();
    p->colo_sock_compare_pri_in_port = rand_str();
    p->colo_sock_compare_sec_in_id = rand_str();
    p->colo_sock_compare_sec_in_ip = rand_str();
    p->colo_sock_compare_sec_in_port = rand_str();
    p->colo_sock_compare_notify_id = rand_str();
    p->colo_sock_compare_notify_ip = rand_str();
    p->colo_sock_compare_notify_port = rand_str();
    p->colo_sock_redirector0_id = rand_str();
    p->colo_sock_redirector0_ip = rand_str();
    p->colo_sock_redirector0_port = rand_str();
    p->colo_sock_redirector1_id = rand_str();
    p->colo_sock_redirector1_ip = rand_str();
    p->colo_sock_redirector1_port = rand_str();
    p->colo_sock_redirector2_id = rand_str();
    p->colo_sock_redirector2_ip = rand_str();
    p->colo_sock_redirector2_port = rand_str();
    p->colo_filter_mirror_queue = rand_str();
    p->colo_filter_mirror_outdev = rand_str();
    p->colo_filter_redirector0_queue = rand_str();
    p->colo_filter_redirector0_indev = rand_str();
    p->colo_filter_redirector0_outdev = rand_str();
    p->colo_filter_redirector1_queue = rand_str();
    p->colo_filter_redirector1_indev = rand_str();
    p->colo_filter_redirector1_outdev = rand_str();
    p->colo_compare_pri_in = rand_str();
    p->colo_compare_sec_in = rand_str();
    p->colo_compare_out = rand_str();
    p->colo_compare_notify_dev = rand_str();
    p->colo_sock_sec_redirector0_id = rand_str();
    p->colo_sock_sec_redirector0_ip = rand_str();
    p->colo_sock_sec_redirector0_port = rand_str();
    p->colo_sock_sec_redirector1_id = rand_str();
    p->colo_sock_sec_redirector1_ip = rand_str();
    p->colo_sock_sec_redirector1_port = rand_str();
    p->colo_filter_sec_redirector0_queue = rand_str();
    p->colo_filter_sec_redirector0_indev = rand_str();
    p->colo_filter_sec_redirector0_outdev = rand_str();
    p->colo_filter_sec_redirector1_queue = rand_str();
    p->colo_filter_sec_redirector1_indev = rand_str();
    p->colo_filter_sec_redirector1_outdev = rand_str();
    p->colo_filter_sec_rewriter0_queue = rand_str();
    p->colo_checkpoint_host = rand_str();
    p->colo_checkpoint_port = rand_str();
}

static void libxl_device_pci_rand_init(libxl_device_pci *p);
static void libxl_device_pci_rand_init(libxl_device_pci *p)
{
    p->func = test_rand(sizeof(p->func) * 8);
    p->dev = test_rand(sizeof(p->dev) * 8);
    p->bus = test_rand(sizeof(p->bus) * 8);
    p->domain = test_rand(sizeof(p->domain) * 8);
    p->vdevfn = test_rand(sizeof(p->vdevfn) * 8);
    p->vfunc_mask = test_rand(sizeof(p->vfunc_mask) * 8);
    p->msitranslate = test_rand(2);
    p->power_mgmt = test_rand(2);
    p->permissive = test_rand(2);
    p->seize = test_rand(2);
    p->rdm_policy = LIBXL_RDM_RESERVE_POLICY_STRICT;
}

static void libxl_device_rdm_rand_init(libxl_device_rdm *p);
static void libxl_device_rdm_rand_init(libxl_device_rdm *p)
{
    p->start = test_rand(sizeof(p->start) * 8);
    p->size = test_rand(sizeof(p->size) * 8);
    p->policy = LIBXL_RDM_RESERVE_POLICY_RELAXED;
}

static void libxl_usbctrl_type_rand_init(libxl_usbctrl_type *p);
static void libxl_usbctrl_type_rand_init(libxl_usbctrl_type *p)
{
    *p = LIBXL_USBCTRL_TYPE_AUTO;
}

static void libxl_usbdev_type_rand_init(libxl_usbdev_type *p);
static void libxl_usbdev_type_rand_init(libxl_usbdev_type *p)
{
    *p = LIBXL_USBDEV_TYPE_HOSTDEV;
}

static void libxl_device_usbctrl_rand_init(libxl_device_usbctrl *p);
static void libxl_device_usbctrl_rand_init(libxl_device_usbctrl *p)
{
    p->type = LIBXL_USBCTRL_TYPE_PV;
    libxl_devid_rand_init(&p->devid);
    p->version = test_rand(sizeof(p->version) * 8);
    p->ports = test_rand(sizeof(p->ports) * 8);
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
}

static void libxl_device_usbdev_rand_init(libxl_device_usbdev *p);
static void libxl_device_usbdev_rand_init(libxl_device_usbdev *p)
{
    libxl_devid_rand_init(&p->ctrl);
    p->port = test_rand(sizeof(p->port) * 8);
    p->type = LIBXL_USBDEV_TYPE_HOSTDEV;
    switch (p->type) {
    case LIBXL_USBDEV_TYPE_HOSTDEV:
        p->u.hostdev.hostbus = test_rand(sizeof(p->u.hostdev.hostbus) * 8);
        p->u.hostdev.hostaddr = test_rand(sizeof(p->u.hostdev.hostaddr) * 8);
        break;
    }
}

static void libxl_device_dtdev_rand_init(libxl_device_dtdev *p);
static void libxl_device_dtdev_rand_init(libxl_device_dtdev *p)
{
    p->path = rand_str();
}

static void libxl_device_vtpm_rand_init(libxl_device_vtpm *p);
static void libxl_device_vtpm_rand_init(libxl_device_vtpm *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    libxl_uuid_rand_init(&p->uuid);
}

static void libxl_device_p9_rand_init(libxl_device_p9 *p);
static void libxl_device_p9_rand_init(libxl_device_p9 *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    p->tag = rand_str();
    p->path = rand_str();
    p->security_model = rand_str();
    libxl_devid_rand_init(&p->devid);
}

static void libxl_device_channel_rand_init(libxl_device_channel *p);
static void libxl_device_channel_rand_init(libxl_device_channel *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    p->name = rand_str();
    p->connection = LIBXL_CHANNEL_CONNECTION_PTY;
    switch (p->connection) {
    case LIBXL_CHANNEL_CONNECTION_UNKNOWN:
        break;
    case LIBXL_CHANNEL_CONNECTION_PTY:
        break;
    case LIBXL_CHANNEL_CONNECTION_SOCKET:
        p->u.socket.path = rand_str();
        break;
    }
}

static void libxl_connector_param_rand_init(libxl_connector_param *p);
static void libxl_connector_param_rand_init(libxl_connector_param *p)
{
    p->id = rand_str();
    p->width = test_rand(sizeof(p->width) * 8);
    p->height = test_rand(sizeof(p->height) * 8);
}

static void libxl_device_vdispl_rand_init(libxl_device_vdispl *p);
static void libxl_device_vdispl_rand_init(libxl_device_vdispl *p)
{
    libxl_domid_rand_init(&p->backend_domid);
    p->backend_domname = rand_str();
    libxl_devid_rand_init(&p->devid);
    p->be_alloc = test_rand(2);
    p->num_connectors = test_rand(8);
    p->connectors = calloc(p->num_connectors, sizeof(*p->connectors));
    assert(p->connectors);
    {
        int i;
        for (i=0; i<p->num_connectors; i++)
            libxl_connector_param_rand_init(&p->connectors[i]);
    }
}

static void libxl_domain_config_rand_init(libxl_domain_config *p);
static void libxl_domain_config_rand_init(libxl_domain_config *p)
{
    libxl_domain_create_info_rand_init(&p->c_info);
    libxl_domain_build_info_rand_init(&p->b_info);
    p->num_disks = test_rand(8);
    p->disks = calloc(p->num_disks, sizeof(*p->disks));
    assert(p->disks);
    {
        int i;
        for (i=0; i<p->num_disks; i++)
            libxl_device_disk_rand_init(&p->disks[i]);
    }
    p->num_nics = test_rand(8);
    p->nics = calloc(p->num_nics, sizeof(*p->nics));
    assert(p->nics);
    {
        int i;
        for (i=0; i<p->num_nics; i++)
            libxl_device_nic_rand_init(&p->nics[i]);
    }
    p->num_pcidevs = test_rand(8);
    p->pcidevs = calloc(p->num_pcidevs, sizeof(*p->pcidevs));
    assert(p->pcidevs);
    {
        int i;
        for (i=0; i<p->num_pcidevs; i++)
            libxl_device_pci_rand_init(&p->pcidevs[i]);
    }
    p->num_rdms = test_rand(8);
    p->rdms = calloc(p->num_rdms, sizeof(*p->rdms));
    assert(p->rdms);
    {
        int i;
        for (i=0; i<p->num_rdms; i++)
            libxl_device_rdm_rand_init(&p->rdms[i]);
    }
    p->num_dtdevs = test_rand(8);
    p->dtdevs = calloc(p->num_dtdevs, sizeof(*p->dtdevs));
    assert(p->dtdevs);
    {
        int i;
        for (i=0; i<p->num_dtdevs; i++)
            libxl_device_dtdev_rand_init(&p->dtdevs[i]);
    }
    p->num_vfbs = test_rand(8);
    p->vfbs = calloc(p->num_vfbs, sizeof(*p->vfbs));
    assert(p->vfbs);
    {
        int i;
        for (i=0; i<p->num_vfbs; i++)
            libxl_device_vfb_rand_init(&p->vfbs[i]);
    }
    p->num_vkbs = test_rand(8);
    p->vkbs = calloc(p->num_vkbs, sizeof(*p->vkbs));
    assert(p->vkbs);
    {
        int i;
        for (i=0; i<p->num_vkbs; i++)
            libxl_device_vkb_rand_init(&p->vkbs[i]);
    }
    p->num_vtpms = test_rand(8);
    p->vtpms = calloc(p->num_vtpms, sizeof(*p->vtpms));
    assert(p->vtpms);
    {
        int i;
        for (i=0; i<p->num_vtpms; i++)
            libxl_device_vtpm_rand_init(&p->vtpms[i]);
    }
    p->num_p9s = test_rand(8);
    p->p9s = calloc(p->num_p9s, sizeof(*p->p9s));
    assert(p->p9s);
    {
        int i;
        for (i=0; i<p->num_p9s; i++)
            libxl_device_p9_rand_init(&p->p9s[i]);
    }
    p->num_vdispls = test_rand(8);
    p->vdispls = calloc(p->num_vdispls, sizeof(*p->vdispls));
    assert(p->vdispls);
    {
        int i;
        for (i=0; i<p->num_vdispls; i++)
            libxl_device_vdispl_rand_init(&p->vdispls[i]);
    }
    p->num_channels = test_rand(8);
    p->channels = calloc(p->num_channels, sizeof(*p->channels));
    assert(p->channels);
    {
        int i;
        for (i=0; i<p->num_channels; i++)
            libxl_device_channel_rand_init(&p->channels[i]);
    }
    p->num_usbctrls = test_rand(8);
    p->usbctrls = calloc(p->num_usbctrls, sizeof(*p->usbctrls));
    assert(p->usbctrls);
    {
        int i;
        for (i=0; i<p->num_usbctrls; i++)
            libxl_device_usbctrl_rand_init(&p->usbctrls[i]);
    }
    p->num_usbdevs = test_rand(8);
    p->usbdevs = calloc(p->num_usbdevs, sizeof(*p->usbdevs));
    assert(p->usbdevs);
    {
        int i;
        for (i=0; i<p->num_usbdevs; i++)
            libxl_device_usbdev_rand_init(&p->usbdevs[i]);
    }
    p->on_poweroff = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY;
    p->on_reboot = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY;
    p->on_watchdog = LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART;
    p->on_crash = LIBXL_ACTION_ON_SHUTDOWN_DESTROY;
    p->on_soft_reset = LIBXL_ACTION_ON_SHUTDOWN_RESTART;
}

static void libxl_diskinfo_rand_init(libxl_diskinfo *p);
static void libxl_diskinfo_rand_init(libxl_diskinfo *p)
{
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    libxl_devid_rand_init(&p->devid);
    p->state = test_rand(sizeof(p->state) * 8);
    p->evtch = test_rand(sizeof(p->evtch) * 8);
    p->rref = test_rand(sizeof(p->rref) * 8);
}

static void libxl_nicinfo_rand_init(libxl_nicinfo *p);
static void libxl_nicinfo_rand_init(libxl_nicinfo *p)
{
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    libxl_devid_rand_init(&p->devid);
    p->state = test_rand(sizeof(p->state) * 8);
    p->evtch = test_rand(sizeof(p->evtch) * 8);
    p->rref_tx = test_rand(sizeof(p->rref_tx) * 8);
    p->rref_rx = test_rand(sizeof(p->rref_rx) * 8);
}

static void libxl_vtpminfo_rand_init(libxl_vtpminfo *p);
static void libxl_vtpminfo_rand_init(libxl_vtpminfo *p)
{
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    libxl_devid_rand_init(&p->devid);
    p->state = test_rand(sizeof(p->state) * 8);
    p->evtch = test_rand(sizeof(p->evtch) * 8);
    p->rref = test_rand(sizeof(p->rref) * 8);
    libxl_uuid_rand_init(&p->uuid);
}

static void libxl_usbctrlinfo_rand_init(libxl_usbctrlinfo *p);
static void libxl_usbctrlinfo_rand_init(libxl_usbctrlinfo *p)
{
    p->type = LIBXL_USBCTRL_TYPE_AUTO;
    libxl_devid_rand_init(&p->devid);
    p->version = test_rand(sizeof(p->version) * 8);
    p->ports = test_rand(sizeof(p->ports) * 8);
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    p->state = test_rand(sizeof(p->state) * 8);
    p->evtch = test_rand(sizeof(p->evtch) * 8);
    p->ref_urb = test_rand(sizeof(p->ref_urb) * 8);
    p->ref_conn = test_rand(sizeof(p->ref_conn) * 8);
}

static void libxl_vcpuinfo_rand_init(libxl_vcpuinfo *p);
static void libxl_vcpuinfo_rand_init(libxl_vcpuinfo *p)
{
    p->vcpuid = test_rand(sizeof(p->vcpuid) * 8);
    p->cpu = test_rand(sizeof(p->cpu) * 8);
    p->online = test_rand(2);
    p->blocked = test_rand(2);
    p->running = test_rand(2);
    p->vcpu_time = test_rand(sizeof(p->vcpu_time) * 8);
    libxl_bitmap_rand_init(&p->cpumap);
    libxl_bitmap_rand_init(&p->cpumap_soft);
}

static void libxl_physinfo_rand_init(libxl_physinfo *p);
static void libxl_physinfo_rand_init(libxl_physinfo *p)
{
    p->domain_unpriv = LIBXL_DOMAIN_UNPRIV_DOM0;
    p->threads_per_core = test_rand(sizeof(p->threads_per_core) * 8);
    p->cores_per_socket = test_rand(sizeof(p->cores_per_socket) * 8);
    p->max_cpu_id = test_rand(sizeof(p->max_cpu_id) * 8);
    p->nr_cpus = test_rand(sizeof(p->nr_cpus) * 8);
    p->cpu_khz = test_rand(sizeof(p->cpu_khz) * 8);
    p->total_pages = test_rand(sizeof(p->total_pages) * 8);
    p->free_pages = test_rand(sizeof(p->free_pages) * 8);
    p->scrub_pages = test_rand(sizeof(p->scrub_pages) * 8);
    p->outstanding_pages = test_rand(sizeof(p->outstanding_pages) * 8);
    p->sharing_freed_pages = test_rand(sizeof(p->sharing_freed_pages) * 8);
    p->sharing_used_frames = test_rand(sizeof(p->sharing_used_frames) * 8);
    p->max_possible_mfn = test_rand(sizeof(p->max_possible_mfn) * 8);
    p->nr_nodes = test_rand(sizeof(p->nr_nodes) * 8);
    libxl_hwcap_rand_init(&p->hw_cap);
    p->cap_hvm = test_rand(2);
    p->cap_hvm_directio = test_rand(2);
}

static void libxl_connectorinfo_rand_init(libxl_connectorinfo *p);
static void libxl_connectorinfo_rand_init(libxl_connectorinfo *p)
{
    p->id = rand_str();
    p->width = test_rand(sizeof(p->width) * 8);
    p->height = test_rand(sizeof(p->height) * 8);
    p->req_evtch = test_rand(sizeof(p->req_evtch) * 8);
    p->req_rref = test_rand(sizeof(p->req_rref) * 8);
    p->evt_evtch = test_rand(sizeof(p->evt_evtch) * 8);
    p->evt_rref = test_rand(sizeof(p->evt_rref) * 8);
}

static void libxl_vdisplinfo_rand_init(libxl_vdisplinfo *p);
static void libxl_vdisplinfo_rand_init(libxl_vdisplinfo *p)
{
    p->backend = rand_str();
    p->backend_id = test_rand(sizeof(p->backend_id) * 8);
    p->frontend = rand_str();
    p->frontend_id = test_rand(sizeof(p->frontend_id) * 8);
    libxl_devid_rand_init(&p->devid);
    p->state = test_rand(sizeof(p->state) * 8);
    p->be_alloc = test_rand(2);
    p->num_connectors = test_rand(8);
    p->connectors = calloc(p->num_connectors, sizeof(*p->connectors));
    assert(p->connectors);
    {
        int i;
        for (i=0; i<p->num_connectors; i++)
            libxl_connectorinfo_rand_init(&p->connectors[i]);
    }
}

static void libxl_numainfo_rand_init(libxl_numainfo *p);
static void libxl_numainfo_rand_init(libxl_numainfo *p)
{
    p->size = test_rand(sizeof(p->size) * 8);
    p->free = test_rand(sizeof(p->free) * 8);
    p->num_dists = test_rand(8);
    p->dists = calloc(p->num_dists, sizeof(*p->dists));
    assert(p->dists);
    {
        int i;
        for (i=0; i<p->num_dists; i++)
            p->dists[i] = test_rand(sizeof(p->dists[i]) * 8);
    }
}

static void libxl_cputopology_rand_init(libxl_cputopology *p);
static void libxl_cputopology_rand_init(libxl_cputopology *p)
{
    p->core = test_rand(sizeof(p->core) * 8);
    p->socket = test_rand(sizeof(p->socket) * 8);
    p->node = test_rand(sizeof(p->node) * 8);
}

static void libxl_pcitopology_rand_init(libxl_pcitopology *p);
static void libxl_pcitopology_rand_init(libxl_pcitopology *p)
{
    p->seg = test_rand(sizeof(p->seg) * 8);
    p->bus = test_rand(sizeof(p->bus) * 8);
    p->devfn = test_rand(sizeof(p->devfn) * 8);
    p->node = test_rand(sizeof(p->node) * 8);
}

static void libxl_sched_credit_params_rand_init(libxl_sched_credit_params *p);
static void libxl_sched_credit_params_rand_init(libxl_sched_credit_params *p)
{
    p->tslice_ms = test_rand(sizeof(p->tslice_ms) * 8);
    p->ratelimit_us = test_rand(sizeof(p->ratelimit_us) * 8);
}

static void libxl_sched_credit2_params_rand_init(libxl_sched_credit2_params *p);
static void libxl_sched_credit2_params_rand_init(libxl_sched_credit2_params *p)
{
    p->ratelimit_us = test_rand(sizeof(p->ratelimit_us) * 8);
}

static void libxl_domain_remus_info_rand_init(libxl_domain_remus_info *p);
static void libxl_domain_remus_info_rand_init(libxl_domain_remus_info *p)
{
    p->interval = test_rand(sizeof(p->interval) * 8);
    libxl_defbool_rand_init(&p->allow_unsafe);
    libxl_defbool_rand_init(&p->blackhole);
    libxl_defbool_rand_init(&p->compression);
    libxl_defbool_rand_init(&p->netbuf);
    p->netbufscript = rand_str();
    libxl_defbool_rand_init(&p->diskbuf);
    libxl_defbool_rand_init(&p->colo);
    libxl_defbool_rand_init(&p->userspace_colo_proxy);
}

static void libxl_event_type_rand_init(libxl_event_type *p);
static void libxl_event_type_rand_init(libxl_event_type *p)
{
    *p = LIBXL_EVENT_TYPE_DISK_EJECT;
}

static void libxl_event_rand_init(libxl_event *p);
static void libxl_event_rand_init(libxl_event *p)
{
    libxl_ev_link_rand_init(&p->link);
    libxl_domid_rand_init(&p->domid);
    libxl_uuid_rand_init(&p->domuuid);
    p->for_user = test_rand(sizeof(p->for_user) * 8);
    p->type = LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN;
    switch (p->type) {
    case LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN:
        p->u.domain_shutdown.shutdown_reason = test_rand(sizeof(p->u.domain_shutdown.shutdown_reason) * 8);
        break;
    case LIBXL_EVENT_TYPE_DOMAIN_DEATH:
        break;
    case LIBXL_EVENT_TYPE_DISK_EJECT:
        p->u.disk_eject.vdev = rand_str();
        libxl_device_disk_rand_init(&p->u.disk_eject.disk);
        break;
    case LIBXL_EVENT_TYPE_OPERATION_COMPLETE:
        p->u.operation_complete.rc = test_rand(sizeof(p->u.operation_complete.rc) * 8);
        break;
    case LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE:
        break;
    }
}

static void libxl_psr_cmt_type_rand_init(libxl_psr_cmt_type *p);
static void libxl_psr_cmt_type_rand_init(libxl_psr_cmt_type *p)
{
    *p = LIBXL_PSR_CMT_TYPE_CACHE_OCCUPANCY;
}

static void libxl_psr_cbm_type_rand_init(libxl_psr_cbm_type *p);
static void libxl_psr_cbm_type_rand_init(libxl_psr_cbm_type *p)
{
    *p = LIBXL_PSR_CBM_TYPE_L2_CBM;
}

static void libxl_psr_cat_info_rand_init(libxl_psr_cat_info *p);
static void libxl_psr_cat_info_rand_init(libxl_psr_cat_info *p)
{
    p->id = test_rand(sizeof(p->id) * 8);
    p->cos_max = test_rand(sizeof(p->cos_max) * 8);
    p->cbm_len = test_rand(sizeof(p->cbm_len) * 8);
    p->cdp_enabled = test_rand(2);
}


int main(int argc, char **argv)
{
    libxl_domain_unpriv libxl_domain_unpriv_val, libxl_domain_unpriv_val_new;
    libxl_error libxl_error_val, libxl_error_val_new;
    libxl_domain_type libxl_domain_type_val, libxl_domain_type_val_new;
    libxl_rdm_reserve_strategy libxl_rdm_reserve_strategy_val, libxl_rdm_reserve_strategy_val_new;
    libxl_rdm_reserve_policy libxl_rdm_reserve_policy_val, libxl_rdm_reserve_policy_val_new;
    libxl_channel_connection libxl_channel_connection_val, libxl_channel_connection_val_new;
    libxl_device_model_version libxl_device_model_version_val, libxl_device_model_version_val_new;
    libxl_console_type libxl_console_type_val, libxl_console_type_val_new;
    libxl_disk_format libxl_disk_format_val, libxl_disk_format_val_new;
    libxl_disk_backend libxl_disk_backend_val, libxl_disk_backend_val_new;
    libxl_nic_type libxl_nic_type_val, libxl_nic_type_val_new;
    libxl_action_on_shutdown libxl_action_on_shutdown_val, libxl_action_on_shutdown_val_new;
    libxl_trigger libxl_trigger_val, libxl_trigger_val_new;
    libxl_tsc_mode libxl_tsc_mode_val, libxl_tsc_mode_val_new;
    libxl_gfx_passthru_kind libxl_gfx_passthru_kind_val, libxl_gfx_passthru_kind_val_new;
    libxl_timer_mode libxl_timer_mode_val, libxl_timer_mode_val_new;
    libxl_bios_type libxl_bios_type_val, libxl_bios_type_val_new;
    libxl_scheduler libxl_scheduler_val, libxl_scheduler_val_new;
    libxl_shutdown_reason libxl_shutdown_reason_val, libxl_shutdown_reason_val_new;
    libxl_vga_interface_type libxl_vga_interface_type_val, libxl_vga_interface_type_val_new;
    libxl_vendor_device libxl_vendor_device_val, libxl_vendor_device_val_new;
    libxl_viridian_enlightenment libxl_viridian_enlightenment_val, libxl_viridian_enlightenment_val_new;
    libxl_hdtype libxl_hdtype_val, libxl_hdtype_val_new;
    libxl_checkpointed_stream libxl_checkpointed_stream_val, libxl_checkpointed_stream_val_new;
    libxl_vuart_type libxl_vuart_type_val, libxl_vuart_type_val_new;
    libxl_ioport_range libxl_ioport_range_val, libxl_ioport_range_val_new;
    libxl_iomem_range libxl_iomem_range_val, libxl_iomem_range_val_new;
    libxl_vga_interface_info libxl_vga_interface_info_val, libxl_vga_interface_info_val_new;
    libxl_vnc_info libxl_vnc_info_val, libxl_vnc_info_val_new;
    libxl_spice_info libxl_spice_info_val, libxl_spice_info_val_new;
    libxl_sdl_info libxl_sdl_info_val, libxl_sdl_info_val_new;
    libxl_dominfo libxl_dominfo_val, libxl_dominfo_val_new;
    libxl_cpupoolinfo libxl_cpupoolinfo_val, libxl_cpupoolinfo_val_new;
    libxl_channelinfo libxl_channelinfo_val, libxl_channelinfo_val_new;
    libxl_vminfo libxl_vminfo_val, libxl_vminfo_val_new;
    libxl_version_info libxl_version_info_val, libxl_version_info_val_new;
    libxl_domain_create_info libxl_domain_create_info_val, libxl_domain_create_info_val_new;
    libxl_domain_restore_params libxl_domain_restore_params_val, libxl_domain_restore_params_val_new;
    libxl_sched_params libxl_sched_params_val, libxl_sched_params_val_new;
    libxl_vcpu_sched_params libxl_vcpu_sched_params_val, libxl_vcpu_sched_params_val_new;
    libxl_domain_sched_params libxl_domain_sched_params_val, libxl_domain_sched_params_val_new;
    libxl_vnode_info libxl_vnode_info_val, libxl_vnode_info_val_new;
    libxl_gic_version libxl_gic_version_val, libxl_gic_version_val_new;
    libxl_rdm_reserve libxl_rdm_reserve_val, libxl_rdm_reserve_val_new;
    libxl_altp2m_mode libxl_altp2m_mode_val, libxl_altp2m_mode_val_new;
    libxl_domain_build_info libxl_domain_build_info_val, libxl_domain_build_info_val_new;
    libxl_device_vfb libxl_device_vfb_val, libxl_device_vfb_val_new;
    libxl_device_vkb libxl_device_vkb_val, libxl_device_vkb_val_new;
    libxl_device_disk libxl_device_disk_val, libxl_device_disk_val_new;
    libxl_device_nic libxl_device_nic_val, libxl_device_nic_val_new;
    libxl_device_pci libxl_device_pci_val, libxl_device_pci_val_new;
    libxl_device_rdm libxl_device_rdm_val, libxl_device_rdm_val_new;
    libxl_usbctrl_type libxl_usbctrl_type_val, libxl_usbctrl_type_val_new;
    libxl_usbdev_type libxl_usbdev_type_val, libxl_usbdev_type_val_new;
    libxl_device_usbctrl libxl_device_usbctrl_val, libxl_device_usbctrl_val_new;
    libxl_device_usbdev libxl_device_usbdev_val, libxl_device_usbdev_val_new;
    libxl_device_dtdev libxl_device_dtdev_val, libxl_device_dtdev_val_new;
    libxl_device_vtpm libxl_device_vtpm_val, libxl_device_vtpm_val_new;
    libxl_device_p9 libxl_device_p9_val, libxl_device_p9_val_new;
    libxl_device_channel libxl_device_channel_val, libxl_device_channel_val_new;
    libxl_connector_param libxl_connector_param_val, libxl_connector_param_val_new;
    libxl_device_vdispl libxl_device_vdispl_val, libxl_device_vdispl_val_new;
    libxl_domain_config libxl_domain_config_val, libxl_domain_config_val_new;
    libxl_diskinfo libxl_diskinfo_val, libxl_diskinfo_val_new;
    libxl_nicinfo libxl_nicinfo_val, libxl_nicinfo_val_new;
    libxl_vtpminfo libxl_vtpminfo_val, libxl_vtpminfo_val_new;
    libxl_usbctrlinfo libxl_usbctrlinfo_val, libxl_usbctrlinfo_val_new;
    libxl_vcpuinfo libxl_vcpuinfo_val, libxl_vcpuinfo_val_new;
    libxl_physinfo libxl_physinfo_val, libxl_physinfo_val_new;
    libxl_connectorinfo libxl_connectorinfo_val, libxl_connectorinfo_val_new;
    libxl_vdisplinfo libxl_vdisplinfo_val, libxl_vdisplinfo_val_new;
    libxl_numainfo libxl_numainfo_val, libxl_numainfo_val_new;
    libxl_cputopology libxl_cputopology_val, libxl_cputopology_val_new;
    libxl_pcitopology libxl_pcitopology_val, libxl_pcitopology_val_new;
    libxl_sched_credit_params libxl_sched_credit_params_val, libxl_sched_credit_params_val_new;
    libxl_sched_credit2_params libxl_sched_credit2_params_val, libxl_sched_credit2_params_val_new;
    libxl_domain_remus_info libxl_domain_remus_info_val, libxl_domain_remus_info_val_new;
    libxl_event_type libxl_event_type_val, libxl_event_type_val_new;
    libxl_event libxl_event_val, libxl_event_val_new;
    libxl_psr_cmt_type libxl_psr_cmt_type_val, libxl_psr_cmt_type_val_new;
    libxl_psr_cbm_type libxl_psr_cbm_type_val, libxl_psr_cbm_type_val_new;
    libxl_psr_cat_info libxl_psr_cat_info_val, libxl_psr_cat_info_val_new;

    int rc;
    char *s, *new_s, *json_string;
    xentoollog_logger_stdiostream *logger;
    libxl_ctx *ctx;

    logger = xtl_createlogger_stdiostream(stderr, XTL_DETAIL, 0);
    if (!logger) exit(1);

    if (libxl_ctx_alloc(&ctx, LIBXL_VERSION, 0, (xentoollog_logger*)logger)) {
        fprintf(stderr, "cannot init xl context\n");
        exit(1);
    }
    printf("Testing TYPE_to/from_json()\n");
    printf("----------------------\n");
    printf("\n");
    libxl_domain_unpriv_rand_init(&libxl_domain_unpriv_val);
    s = libxl_domain_unpriv_to_json(ctx, libxl_domain_unpriv_val);
    printf("%s: %s\n", "libxl_domain_unpriv", s);
    if (s == NULL) abort();
    rc = libxl_domain_unpriv_from_json(ctx, &libxl_domain_unpriv_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_unpriv_to_json(ctx, libxl_domain_unpriv_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_error_rand_init(&libxl_error_val);
    s = libxl_error_to_json(ctx, libxl_error_val);
    printf("%s: %s\n", "libxl_error", s);
    if (s == NULL) abort();
    rc = libxl_error_from_json(ctx, &libxl_error_val_new, s);
    if (rc) abort();
    new_s = libxl_error_to_json(ctx, libxl_error_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_domain_type_rand_init(&libxl_domain_type_val);
    s = libxl_domain_type_to_json(ctx, libxl_domain_type_val);
    printf("%s: %s\n", "libxl_domain_type", s);
    if (s == NULL) abort();
    rc = libxl_domain_type_from_json(ctx, &libxl_domain_type_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_type_to_json(ctx, libxl_domain_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_rdm_reserve_strategy_rand_init(&libxl_rdm_reserve_strategy_val);
    s = libxl_rdm_reserve_strategy_to_json(ctx, libxl_rdm_reserve_strategy_val);
    printf("%s: %s\n", "libxl_rdm_reserve_strategy", s);
    if (s == NULL) abort();
    rc = libxl_rdm_reserve_strategy_from_json(ctx, &libxl_rdm_reserve_strategy_val_new, s);
    if (rc) abort();
    new_s = libxl_rdm_reserve_strategy_to_json(ctx, libxl_rdm_reserve_strategy_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_rdm_reserve_policy_rand_init(&libxl_rdm_reserve_policy_val);
    s = libxl_rdm_reserve_policy_to_json(ctx, libxl_rdm_reserve_policy_val);
    printf("%s: %s\n", "libxl_rdm_reserve_policy", s);
    if (s == NULL) abort();
    rc = libxl_rdm_reserve_policy_from_json(ctx, &libxl_rdm_reserve_policy_val_new, s);
    if (rc) abort();
    new_s = libxl_rdm_reserve_policy_to_json(ctx, libxl_rdm_reserve_policy_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_channel_connection_rand_init(&libxl_channel_connection_val);
    s = libxl_channel_connection_to_json(ctx, libxl_channel_connection_val);
    printf("%s: %s\n", "libxl_channel_connection", s);
    if (s == NULL) abort();
    rc = libxl_channel_connection_from_json(ctx, &libxl_channel_connection_val_new, s);
    if (rc) abort();
    new_s = libxl_channel_connection_to_json(ctx, libxl_channel_connection_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_device_model_version_rand_init(&libxl_device_model_version_val);
    s = libxl_device_model_version_to_json(ctx, libxl_device_model_version_val);
    printf("%s: %s\n", "libxl_device_model_version", s);
    if (s == NULL) abort();
    rc = libxl_device_model_version_from_json(ctx, &libxl_device_model_version_val_new, s);
    if (rc) abort();
    new_s = libxl_device_model_version_to_json(ctx, libxl_device_model_version_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_console_type_rand_init(&libxl_console_type_val);
    s = libxl_console_type_to_json(ctx, libxl_console_type_val);
    printf("%s: %s\n", "libxl_console_type", s);
    if (s == NULL) abort();
    rc = libxl_console_type_from_json(ctx, &libxl_console_type_val_new, s);
    if (rc) abort();
    new_s = libxl_console_type_to_json(ctx, libxl_console_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_disk_format_rand_init(&libxl_disk_format_val);
    s = libxl_disk_format_to_json(ctx, libxl_disk_format_val);
    printf("%s: %s\n", "libxl_disk_format", s);
    if (s == NULL) abort();
    rc = libxl_disk_format_from_json(ctx, &libxl_disk_format_val_new, s);
    if (rc) abort();
    new_s = libxl_disk_format_to_json(ctx, libxl_disk_format_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_disk_backend_rand_init(&libxl_disk_backend_val);
    s = libxl_disk_backend_to_json(ctx, libxl_disk_backend_val);
    printf("%s: %s\n", "libxl_disk_backend", s);
    if (s == NULL) abort();
    rc = libxl_disk_backend_from_json(ctx, &libxl_disk_backend_val_new, s);
    if (rc) abort();
    new_s = libxl_disk_backend_to_json(ctx, libxl_disk_backend_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_nic_type_rand_init(&libxl_nic_type_val);
    s = libxl_nic_type_to_json(ctx, libxl_nic_type_val);
    printf("%s: %s\n", "libxl_nic_type", s);
    if (s == NULL) abort();
    rc = libxl_nic_type_from_json(ctx, &libxl_nic_type_val_new, s);
    if (rc) abort();
    new_s = libxl_nic_type_to_json(ctx, libxl_nic_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_action_on_shutdown_rand_init(&libxl_action_on_shutdown_val);
    s = libxl_action_on_shutdown_to_json(ctx, libxl_action_on_shutdown_val);
    printf("%s: %s\n", "libxl_action_on_shutdown", s);
    if (s == NULL) abort();
    rc = libxl_action_on_shutdown_from_json(ctx, &libxl_action_on_shutdown_val_new, s);
    if (rc) abort();
    new_s = libxl_action_on_shutdown_to_json(ctx, libxl_action_on_shutdown_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_trigger_rand_init(&libxl_trigger_val);
    s = libxl_trigger_to_json(ctx, libxl_trigger_val);
    printf("%s: %s\n", "libxl_trigger", s);
    if (s == NULL) abort();
    rc = libxl_trigger_from_json(ctx, &libxl_trigger_val_new, s);
    if (rc) abort();
    new_s = libxl_trigger_to_json(ctx, libxl_trigger_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_tsc_mode_rand_init(&libxl_tsc_mode_val);
    s = libxl_tsc_mode_to_json(ctx, libxl_tsc_mode_val);
    printf("%s: %s\n", "libxl_tsc_mode", s);
    if (s == NULL) abort();
    rc = libxl_tsc_mode_from_json(ctx, &libxl_tsc_mode_val_new, s);
    if (rc) abort();
    new_s = libxl_tsc_mode_to_json(ctx, libxl_tsc_mode_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_gfx_passthru_kind_rand_init(&libxl_gfx_passthru_kind_val);
    s = libxl_gfx_passthru_kind_to_json(ctx, libxl_gfx_passthru_kind_val);
    printf("%s: %s\n", "libxl_gfx_passthru_kind", s);
    if (s == NULL) abort();
    rc = libxl_gfx_passthru_kind_from_json(ctx, &libxl_gfx_passthru_kind_val_new, s);
    if (rc) abort();
    new_s = libxl_gfx_passthru_kind_to_json(ctx, libxl_gfx_passthru_kind_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_timer_mode_rand_init(&libxl_timer_mode_val);
    s = libxl_timer_mode_to_json(ctx, libxl_timer_mode_val);
    printf("%s: %s\n", "libxl_timer_mode", s);
    if (s == NULL) abort();
    rc = libxl_timer_mode_from_json(ctx, &libxl_timer_mode_val_new, s);
    if (rc) abort();
    new_s = libxl_timer_mode_to_json(ctx, libxl_timer_mode_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_bios_type_rand_init(&libxl_bios_type_val);
    s = libxl_bios_type_to_json(ctx, libxl_bios_type_val);
    printf("%s: %s\n", "libxl_bios_type", s);
    if (s == NULL) abort();
    rc = libxl_bios_type_from_json(ctx, &libxl_bios_type_val_new, s);
    if (rc) abort();
    new_s = libxl_bios_type_to_json(ctx, libxl_bios_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_scheduler_rand_init(&libxl_scheduler_val);
    s = libxl_scheduler_to_json(ctx, libxl_scheduler_val);
    printf("%s: %s\n", "libxl_scheduler", s);
    if (s == NULL) abort();
    rc = libxl_scheduler_from_json(ctx, &libxl_scheduler_val_new, s);
    if (rc) abort();
    new_s = libxl_scheduler_to_json(ctx, libxl_scheduler_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_shutdown_reason_rand_init(&libxl_shutdown_reason_val);
    s = libxl_shutdown_reason_to_json(ctx, libxl_shutdown_reason_val);
    printf("%s: %s\n", "libxl_shutdown_reason", s);
    if (s == NULL) abort();
    rc = libxl_shutdown_reason_from_json(ctx, &libxl_shutdown_reason_val_new, s);
    if (rc) abort();
    new_s = libxl_shutdown_reason_to_json(ctx, libxl_shutdown_reason_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_vga_interface_type_rand_init(&libxl_vga_interface_type_val);
    s = libxl_vga_interface_type_to_json(ctx, libxl_vga_interface_type_val);
    printf("%s: %s\n", "libxl_vga_interface_type", s);
    if (s == NULL) abort();
    rc = libxl_vga_interface_type_from_json(ctx, &libxl_vga_interface_type_val_new, s);
    if (rc) abort();
    new_s = libxl_vga_interface_type_to_json(ctx, libxl_vga_interface_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_vendor_device_rand_init(&libxl_vendor_device_val);
    s = libxl_vendor_device_to_json(ctx, libxl_vendor_device_val);
    printf("%s: %s\n", "libxl_vendor_device", s);
    if (s == NULL) abort();
    rc = libxl_vendor_device_from_json(ctx, &libxl_vendor_device_val_new, s);
    if (rc) abort();
    new_s = libxl_vendor_device_to_json(ctx, libxl_vendor_device_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_viridian_enlightenment_rand_init(&libxl_viridian_enlightenment_val);
    s = libxl_viridian_enlightenment_to_json(ctx, libxl_viridian_enlightenment_val);
    printf("%s: %s\n", "libxl_viridian_enlightenment", s);
    if (s == NULL) abort();
    rc = libxl_viridian_enlightenment_from_json(ctx, &libxl_viridian_enlightenment_val_new, s);
    if (rc) abort();
    new_s = libxl_viridian_enlightenment_to_json(ctx, libxl_viridian_enlightenment_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_hdtype_rand_init(&libxl_hdtype_val);
    s = libxl_hdtype_to_json(ctx, libxl_hdtype_val);
    printf("%s: %s\n", "libxl_hdtype", s);
    if (s == NULL) abort();
    rc = libxl_hdtype_from_json(ctx, &libxl_hdtype_val_new, s);
    if (rc) abort();
    new_s = libxl_hdtype_to_json(ctx, libxl_hdtype_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_checkpointed_stream_rand_init(&libxl_checkpointed_stream_val);
    s = libxl_checkpointed_stream_to_json(ctx, libxl_checkpointed_stream_val);
    printf("%s: %s\n", "libxl_checkpointed_stream", s);
    if (s == NULL) abort();
    rc = libxl_checkpointed_stream_from_json(ctx, &libxl_checkpointed_stream_val_new, s);
    if (rc) abort();
    new_s = libxl_checkpointed_stream_to_json(ctx, libxl_checkpointed_stream_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_vuart_type_rand_init(&libxl_vuart_type_val);
    s = libxl_vuart_type_to_json(ctx, libxl_vuart_type_val);
    printf("%s: %s\n", "libxl_vuart_type", s);
    if (s == NULL) abort();
    rc = libxl_vuart_type_from_json(ctx, &libxl_vuart_type_val_new, s);
    if (rc) abort();
    new_s = libxl_vuart_type_to_json(ctx, libxl_vuart_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_ioport_range_rand_init(&libxl_ioport_range_val);
    libxl_ioport_range_init(&libxl_ioport_range_val_new);
    libxl_ioport_range_init(&libxl_ioport_range_val_new);
    libxl_ioport_range_init(&libxl_ioport_range_val_new);
    libxl_ioport_range_init(&libxl_ioport_range_val_new);
    s = libxl_ioport_range_to_json(ctx, &libxl_ioport_range_val);
    printf("%s: %s\n", "libxl_ioport_range", s);
    if (s == NULL) abort();
    rc = libxl_ioport_range_from_json(ctx, &libxl_ioport_range_val_new, s);
    if (rc) abort();
    new_s = libxl_ioport_range_to_json(ctx, &libxl_ioport_range_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_ioport_range_dispose(&libxl_ioport_range_val);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);

    libxl_iomem_range_rand_init(&libxl_iomem_range_val);
    libxl_iomem_range_init(&libxl_iomem_range_val_new);
    s = libxl_iomem_range_to_json(ctx, &libxl_iomem_range_val);
    printf("%s: %s\n", "libxl_iomem_range", s);
    if (s == NULL) abort();
    rc = libxl_iomem_range_from_json(ctx, &libxl_iomem_range_val_new, s);
    if (rc) abort();
    new_s = libxl_iomem_range_to_json(ctx, &libxl_iomem_range_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_iomem_range_dispose(&libxl_iomem_range_val);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);

    libxl_vga_interface_info_rand_init(&libxl_vga_interface_info_val);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    s = libxl_vga_interface_info_to_json(ctx, &libxl_vga_interface_info_val);
    printf("%s: %s\n", "libxl_vga_interface_info", s);
    if (s == NULL) abort();
    rc = libxl_vga_interface_info_from_json(ctx, &libxl_vga_interface_info_val_new, s);
    if (rc) abort();
    new_s = libxl_vga_interface_info_to_json(ctx, &libxl_vga_interface_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);

    libxl_vnc_info_rand_init(&libxl_vnc_info_val);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    s = libxl_vnc_info_to_json(ctx, &libxl_vnc_info_val);
    printf("%s: %s\n", "libxl_vnc_info", s);
    if (s == NULL) abort();
    rc = libxl_vnc_info_from_json(ctx, &libxl_vnc_info_val_new, s);
    if (rc) abort();
    new_s = libxl_vnc_info_to_json(ctx, &libxl_vnc_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vnc_info_dispose(&libxl_vnc_info_val);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);

    libxl_spice_info_rand_init(&libxl_spice_info_val);
    libxl_spice_info_init(&libxl_spice_info_val_new);
    libxl_spice_info_init(&libxl_spice_info_val_new);
    s = libxl_spice_info_to_json(ctx, &libxl_spice_info_val);
    printf("%s: %s\n", "libxl_spice_info", s);
    if (s == NULL) abort();
    rc = libxl_spice_info_from_json(ctx, &libxl_spice_info_val_new, s);
    if (rc) abort();
    new_s = libxl_spice_info_to_json(ctx, &libxl_spice_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_spice_info_dispose(&libxl_spice_info_val);
    libxl_spice_info_dispose(&libxl_spice_info_val_new);
    libxl_spice_info_dispose(&libxl_spice_info_val_new);

    libxl_sdl_info_rand_init(&libxl_sdl_info_val);
    libxl_sdl_info_init(&libxl_sdl_info_val_new);
    libxl_sdl_info_init(&libxl_sdl_info_val_new);
    libxl_sdl_info_init(&libxl_sdl_info_val_new);
    libxl_sdl_info_init(&libxl_sdl_info_val_new);
    s = libxl_sdl_info_to_json(ctx, &libxl_sdl_info_val);
    printf("%s: %s\n", "libxl_sdl_info", s);
    if (s == NULL) abort();
    rc = libxl_sdl_info_from_json(ctx, &libxl_sdl_info_val_new, s);
    if (rc) abort();
    new_s = libxl_sdl_info_to_json(ctx, &libxl_sdl_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_sdl_info_dispose(&libxl_sdl_info_val);
    libxl_sdl_info_dispose(&libxl_sdl_info_val_new);

    libxl_dominfo_rand_init(&libxl_dominfo_val);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    s = libxl_dominfo_to_json(ctx, &libxl_dominfo_val);
    printf("%s: %s\n", "libxl_dominfo", s);
    if (s == NULL) abort();
    rc = libxl_dominfo_from_json(ctx, &libxl_dominfo_val_new, s);
    if (rc) abort();
    new_s = libxl_dominfo_to_json(ctx, &libxl_dominfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_dominfo_dispose(&libxl_dominfo_val);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);

    libxl_cpupoolinfo_rand_init(&libxl_cpupoolinfo_val);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    s = libxl_cpupoolinfo_to_json(ctx, &libxl_cpupoolinfo_val);
    printf("%s: %s\n", "libxl_cpupoolinfo", s);
    if (s == NULL) abort();
    rc = libxl_cpupoolinfo_from_json(ctx, &libxl_cpupoolinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_cpupoolinfo_to_json(ctx, &libxl_cpupoolinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val_new);

    libxl_channelinfo_rand_init(&libxl_channelinfo_val);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    s = libxl_channelinfo_to_json(ctx, &libxl_channelinfo_val);
    printf("%s: %s\n", "libxl_channelinfo", s);
    if (s == NULL) abort();
    rc = libxl_channelinfo_from_json(ctx, &libxl_channelinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_channelinfo_to_json(ctx, &libxl_channelinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_channelinfo_dispose(&libxl_channelinfo_val);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);

    libxl_vminfo_rand_init(&libxl_vminfo_val);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    s = libxl_vminfo_to_json(ctx, &libxl_vminfo_val);
    printf("%s: %s\n", "libxl_vminfo", s);
    if (s == NULL) abort();
    rc = libxl_vminfo_from_json(ctx, &libxl_vminfo_val_new, s);
    if (rc) abort();
    new_s = libxl_vminfo_to_json(ctx, &libxl_vminfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vminfo_dispose(&libxl_vminfo_val);
    libxl_vminfo_dispose(&libxl_vminfo_val_new);
    libxl_vminfo_dispose(&libxl_vminfo_val_new);

    libxl_version_info_rand_init(&libxl_version_info_val);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_init(&libxl_version_info_val_new);
    s = libxl_version_info_to_json(ctx, &libxl_version_info_val);
    printf("%s: %s\n", "libxl_version_info", s);
    if (s == NULL) abort();
    rc = libxl_version_info_from_json(ctx, &libxl_version_info_val_new, s);
    if (rc) abort();
    new_s = libxl_version_info_to_json(ctx, &libxl_version_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_version_info_dispose(&libxl_version_info_val);
    libxl_version_info_dispose(&libxl_version_info_val_new);
    libxl_version_info_dispose(&libxl_version_info_val_new);

    libxl_domain_create_info_rand_init(&libxl_domain_create_info_val);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    s = libxl_domain_create_info_to_json(ctx, &libxl_domain_create_info_val);
    printf("%s: %s\n", "libxl_domain_create_info", s);
    if (s == NULL) abort();
    rc = libxl_domain_create_info_from_json(ctx, &libxl_domain_create_info_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_create_info_to_json(ctx, &libxl_domain_create_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);

    libxl_domain_restore_params_rand_init(&libxl_domain_restore_params_val);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    s = libxl_domain_restore_params_to_json(ctx, &libxl_domain_restore_params_val);
    printf("%s: %s\n", "libxl_domain_restore_params", s);
    if (s == NULL) abort();
    rc = libxl_domain_restore_params_from_json(ctx, &libxl_domain_restore_params_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_restore_params_to_json(ctx, &libxl_domain_restore_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);

    libxl_sched_params_rand_init(&libxl_sched_params_val);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    s = libxl_sched_params_to_json(ctx, &libxl_sched_params_val);
    printf("%s: %s\n", "libxl_sched_params", s);
    if (s == NULL) abort();
    rc = libxl_sched_params_from_json(ctx, &libxl_sched_params_val_new, s);
    if (rc) abort();
    new_s = libxl_sched_params_to_json(ctx, &libxl_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_sched_params_dispose(&libxl_sched_params_val);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);

    libxl_vcpu_sched_params_rand_init(&libxl_vcpu_sched_params_val);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    s = libxl_vcpu_sched_params_to_json(ctx, &libxl_vcpu_sched_params_val);
    printf("%s: %s\n", "libxl_vcpu_sched_params", s);
    if (s == NULL) abort();
    rc = libxl_vcpu_sched_params_from_json(ctx, &libxl_vcpu_sched_params_val_new, s);
    if (rc) abort();
    new_s = libxl_vcpu_sched_params_to_json(ctx, &libxl_vcpu_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vcpu_sched_params_dispose(&libxl_vcpu_sched_params_val);
    libxl_vcpu_sched_params_dispose(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_dispose(&libxl_vcpu_sched_params_val_new);

    libxl_domain_sched_params_rand_init(&libxl_domain_sched_params_val);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    s = libxl_domain_sched_params_to_json(ctx, &libxl_domain_sched_params_val);
    printf("%s: %s\n", "libxl_domain_sched_params", s);
    if (s == NULL) abort();
    rc = libxl_domain_sched_params_from_json(ctx, &libxl_domain_sched_params_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_sched_params_to_json(ctx, &libxl_domain_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);

    libxl_vnode_info_rand_init(&libxl_vnode_info_val);
    libxl_vnode_info_init(&libxl_vnode_info_val_new);
    libxl_vnode_info_init(&libxl_vnode_info_val_new);
    libxl_vnode_info_init(&libxl_vnode_info_val_new);
    libxl_vnode_info_init(&libxl_vnode_info_val_new);
    s = libxl_vnode_info_to_json(ctx, &libxl_vnode_info_val);
    printf("%s: %s\n", "libxl_vnode_info", s);
    if (s == NULL) abort();
    rc = libxl_vnode_info_from_json(ctx, &libxl_vnode_info_val_new, s);
    if (rc) abort();
    new_s = libxl_vnode_info_to_json(ctx, &libxl_vnode_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vnode_info_dispose(&libxl_vnode_info_val);
    libxl_vnode_info_dispose(&libxl_vnode_info_val_new);
    libxl_vnode_info_dispose(&libxl_vnode_info_val_new);
    libxl_vnode_info_dispose(&libxl_vnode_info_val_new);

    libxl_gic_version_rand_init(&libxl_gic_version_val);
    s = libxl_gic_version_to_json(ctx, libxl_gic_version_val);
    printf("%s: %s\n", "libxl_gic_version", s);
    if (s == NULL) abort();
    rc = libxl_gic_version_from_json(ctx, &libxl_gic_version_val_new, s);
    if (rc) abort();
    new_s = libxl_gic_version_to_json(ctx, libxl_gic_version_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_rdm_reserve_rand_init(&libxl_rdm_reserve_val);
    libxl_rdm_reserve_init(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_init(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_init(&libxl_rdm_reserve_val_new);
    s = libxl_rdm_reserve_to_json(ctx, &libxl_rdm_reserve_val);
    printf("%s: %s\n", "libxl_rdm_reserve", s);
    if (s == NULL) abort();
    rc = libxl_rdm_reserve_from_json(ctx, &libxl_rdm_reserve_val_new, s);
    if (rc) abort();
    new_s = libxl_rdm_reserve_to_json(ctx, &libxl_rdm_reserve_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);

    libxl_altp2m_mode_rand_init(&libxl_altp2m_mode_val);
    s = libxl_altp2m_mode_to_json(ctx, libxl_altp2m_mode_val);
    printf("%s: %s\n", "libxl_altp2m_mode", s);
    if (s == NULL) abort();
    rc = libxl_altp2m_mode_from_json(ctx, &libxl_altp2m_mode_val_new, s);
    if (rc) abort();
    new_s = libxl_altp2m_mode_to_json(ctx, libxl_altp2m_mode_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_domain_build_info_rand_init(&libxl_domain_build_info_val);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    s = libxl_domain_build_info_to_json(ctx, &libxl_domain_build_info_val);
    printf("%s: %s\n", "libxl_domain_build_info", s);
    if (s == NULL) abort();
    rc = libxl_domain_build_info_from_json(ctx, &libxl_domain_build_info_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_build_info_to_json(ctx, &libxl_domain_build_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val_new);

    libxl_device_vfb_rand_init(&libxl_device_vfb_val);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    s = libxl_device_vfb_to_json(ctx, &libxl_device_vfb_val);
    printf("%s: %s\n", "libxl_device_vfb", s);
    if (s == NULL) abort();
    rc = libxl_device_vfb_from_json(ctx, &libxl_device_vfb_val_new, s);
    if (rc) abort();
    new_s = libxl_device_vfb_to_json(ctx, &libxl_device_vfb_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vfb_dispose(&libxl_device_vfb_val);
    libxl_device_vfb_dispose(&libxl_device_vfb_val_new);
    libxl_device_vfb_dispose(&libxl_device_vfb_val_new);

    libxl_device_vkb_rand_init(&libxl_device_vkb_val);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    s = libxl_device_vkb_to_json(ctx, &libxl_device_vkb_val);
    printf("%s: %s\n", "libxl_device_vkb", s);
    if (s == NULL) abort();
    rc = libxl_device_vkb_from_json(ctx, &libxl_device_vkb_val_new, s);
    if (rc) abort();
    new_s = libxl_device_vkb_to_json(ctx, &libxl_device_vkb_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vkb_dispose(&libxl_device_vkb_val);
    libxl_device_vkb_dispose(&libxl_device_vkb_val_new);
    libxl_device_vkb_dispose(&libxl_device_vkb_val_new);
    libxl_device_vkb_dispose(&libxl_device_vkb_val_new);
    libxl_device_vkb_dispose(&libxl_device_vkb_val_new);

    libxl_device_disk_rand_init(&libxl_device_disk_val);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    s = libxl_device_disk_to_json(ctx, &libxl_device_disk_val);
    printf("%s: %s\n", "libxl_device_disk", s);
    if (s == NULL) abort();
    rc = libxl_device_disk_from_json(ctx, &libxl_device_disk_val_new, s);
    if (rc) abort();
    new_s = libxl_device_disk_to_json(ctx, &libxl_device_disk_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_disk_dispose(&libxl_device_disk_val);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);

    libxl_device_nic_rand_init(&libxl_device_nic_val);
    libxl_device_nic_init(&libxl_device_nic_val_new);
    libxl_device_nic_init(&libxl_device_nic_val_new);
    libxl_device_nic_init(&libxl_device_nic_val_new);
    s = libxl_device_nic_to_json(ctx, &libxl_device_nic_val);
    printf("%s: %s\n", "libxl_device_nic", s);
    if (s == NULL) abort();
    rc = libxl_device_nic_from_json(ctx, &libxl_device_nic_val_new, s);
    if (rc) abort();
    new_s = libxl_device_nic_to_json(ctx, &libxl_device_nic_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_nic_dispose(&libxl_device_nic_val);
    libxl_device_nic_dispose(&libxl_device_nic_val_new);
    libxl_device_nic_dispose(&libxl_device_nic_val_new);

    libxl_device_pci_rand_init(&libxl_device_pci_val);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    s = libxl_device_pci_to_json(ctx, &libxl_device_pci_val);
    printf("%s: %s\n", "libxl_device_pci", s);
    if (s == NULL) abort();
    rc = libxl_device_pci_from_json(ctx, &libxl_device_pci_val_new, s);
    if (rc) abort();
    new_s = libxl_device_pci_to_json(ctx, &libxl_device_pci_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_pci_dispose(&libxl_device_pci_val);
    libxl_device_pci_dispose(&libxl_device_pci_val_new);
    libxl_device_pci_dispose(&libxl_device_pci_val_new);
    libxl_device_pci_dispose(&libxl_device_pci_val_new);
    libxl_device_pci_dispose(&libxl_device_pci_val_new);

    libxl_device_rdm_rand_init(&libxl_device_rdm_val);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    s = libxl_device_rdm_to_json(ctx, &libxl_device_rdm_val);
    printf("%s: %s\n", "libxl_device_rdm", s);
    if (s == NULL) abort();
    rc = libxl_device_rdm_from_json(ctx, &libxl_device_rdm_val_new, s);
    if (rc) abort();
    new_s = libxl_device_rdm_to_json(ctx, &libxl_device_rdm_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_rdm_dispose(&libxl_device_rdm_val);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);

    libxl_usbctrl_type_rand_init(&libxl_usbctrl_type_val);
    s = libxl_usbctrl_type_to_json(ctx, libxl_usbctrl_type_val);
    printf("%s: %s\n", "libxl_usbctrl_type", s);
    if (s == NULL) abort();
    rc = libxl_usbctrl_type_from_json(ctx, &libxl_usbctrl_type_val_new, s);
    if (rc) abort();
    new_s = libxl_usbctrl_type_to_json(ctx, libxl_usbctrl_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_usbdev_type_rand_init(&libxl_usbdev_type_val);
    s = libxl_usbdev_type_to_json(ctx, libxl_usbdev_type_val);
    printf("%s: %s\n", "libxl_usbdev_type", s);
    if (s == NULL) abort();
    rc = libxl_usbdev_type_from_json(ctx, &libxl_usbdev_type_val_new, s);
    if (rc) abort();
    new_s = libxl_usbdev_type_to_json(ctx, libxl_usbdev_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_device_usbctrl_rand_init(&libxl_device_usbctrl_val);
    libxl_device_usbctrl_init(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_init(&libxl_device_usbctrl_val_new);
    s = libxl_device_usbctrl_to_json(ctx, &libxl_device_usbctrl_val);
    printf("%s: %s\n", "libxl_device_usbctrl", s);
    if (s == NULL) abort();
    rc = libxl_device_usbctrl_from_json(ctx, &libxl_device_usbctrl_val_new, s);
    if (rc) abort();
    new_s = libxl_device_usbctrl_to_json(ctx, &libxl_device_usbctrl_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);

    libxl_device_usbdev_rand_init(&libxl_device_usbdev_val);
    libxl_device_usbdev_init(&libxl_device_usbdev_val_new);
    libxl_device_usbdev_init(&libxl_device_usbdev_val_new);
    libxl_device_usbdev_init(&libxl_device_usbdev_val_new);
    s = libxl_device_usbdev_to_json(ctx, &libxl_device_usbdev_val);
    printf("%s: %s\n", "libxl_device_usbdev", s);
    if (s == NULL) abort();
    rc = libxl_device_usbdev_from_json(ctx, &libxl_device_usbdev_val_new, s);
    if (rc) abort();
    new_s = libxl_device_usbdev_to_json(ctx, &libxl_device_usbdev_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_usbdev_dispose(&libxl_device_usbdev_val);
    libxl_device_usbdev_dispose(&libxl_device_usbdev_val_new);
    libxl_device_usbdev_dispose(&libxl_device_usbdev_val_new);

    libxl_device_dtdev_rand_init(&libxl_device_dtdev_val);
    libxl_device_dtdev_init(&libxl_device_dtdev_val_new);
    libxl_device_dtdev_init(&libxl_device_dtdev_val_new);
    s = libxl_device_dtdev_to_json(ctx, &libxl_device_dtdev_val);
    printf("%s: %s\n", "libxl_device_dtdev", s);
    if (s == NULL) abort();
    rc = libxl_device_dtdev_from_json(ctx, &libxl_device_dtdev_val_new, s);
    if (rc) abort();
    new_s = libxl_device_dtdev_to_json(ctx, &libxl_device_dtdev_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val_new);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val_new);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val_new);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val_new);

    libxl_device_vtpm_rand_init(&libxl_device_vtpm_val);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    s = libxl_device_vtpm_to_json(ctx, &libxl_device_vtpm_val);
    printf("%s: %s\n", "libxl_device_vtpm", s);
    if (s == NULL) abort();
    rc = libxl_device_vtpm_from_json(ctx, &libxl_device_vtpm_val_new, s);
    if (rc) abort();
    new_s = libxl_device_vtpm_to_json(ctx, &libxl_device_vtpm_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val_new);

    libxl_device_p9_rand_init(&libxl_device_p9_val);
    libxl_device_p9_init(&libxl_device_p9_val_new);
    libxl_device_p9_init(&libxl_device_p9_val_new);
    libxl_device_p9_init(&libxl_device_p9_val_new);
    libxl_device_p9_init(&libxl_device_p9_val_new);
    s = libxl_device_p9_to_json(ctx, &libxl_device_p9_val);
    printf("%s: %s\n", "libxl_device_p9", s);
    if (s == NULL) abort();
    rc = libxl_device_p9_from_json(ctx, &libxl_device_p9_val_new, s);
    if (rc) abort();
    new_s = libxl_device_p9_to_json(ctx, &libxl_device_p9_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_p9_dispose(&libxl_device_p9_val);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);

    libxl_device_channel_rand_init(&libxl_device_channel_val);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    s = libxl_device_channel_to_json(ctx, &libxl_device_channel_val);
    printf("%s: %s\n", "libxl_device_channel", s);
    if (s == NULL) abort();
    rc = libxl_device_channel_from_json(ctx, &libxl_device_channel_val_new, s);
    if (rc) abort();
    new_s = libxl_device_channel_to_json(ctx, &libxl_device_channel_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_channel_dispose(&libxl_device_channel_val);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);

    libxl_connector_param_rand_init(&libxl_connector_param_val);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    s = libxl_connector_param_to_json(ctx, &libxl_connector_param_val);
    printf("%s: %s\n", "libxl_connector_param", s);
    if (s == NULL) abort();
    rc = libxl_connector_param_from_json(ctx, &libxl_connector_param_val_new, s);
    if (rc) abort();
    new_s = libxl_connector_param_to_json(ctx, &libxl_connector_param_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_connector_param_dispose(&libxl_connector_param_val);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);

    libxl_device_vdispl_rand_init(&libxl_device_vdispl_val);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    s = libxl_device_vdispl_to_json(ctx, &libxl_device_vdispl_val);
    printf("%s: %s\n", "libxl_device_vdispl", s);
    if (s == NULL) abort();
    rc = libxl_device_vdispl_from_json(ctx, &libxl_device_vdispl_val_new, s);
    if (rc) abort();
    new_s = libxl_device_vdispl_to_json(ctx, &libxl_device_vdispl_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);

    libxl_domain_config_rand_init(&libxl_domain_config_val);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    s = libxl_domain_config_to_json(ctx, &libxl_domain_config_val);
    printf("%s: %s\n", "libxl_domain_config", s);
    if (s == NULL) abort();
    rc = libxl_domain_config_from_json(ctx, &libxl_domain_config_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_config_to_json(ctx, &libxl_domain_config_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_config_dispose(&libxl_domain_config_val);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);

    libxl_diskinfo_rand_init(&libxl_diskinfo_val);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    s = libxl_diskinfo_to_json(ctx, &libxl_diskinfo_val);
    printf("%s: %s\n", "libxl_diskinfo", s);
    if (s == NULL) abort();
    rc = libxl_diskinfo_from_json(ctx, &libxl_diskinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_diskinfo_to_json(ctx, &libxl_diskinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_diskinfo_dispose(&libxl_diskinfo_val);
    libxl_diskinfo_dispose(&libxl_diskinfo_val_new);
    libxl_diskinfo_dispose(&libxl_diskinfo_val_new);
    libxl_diskinfo_dispose(&libxl_diskinfo_val_new);

    libxl_nicinfo_rand_init(&libxl_nicinfo_val);
    libxl_nicinfo_init(&libxl_nicinfo_val_new);
    s = libxl_nicinfo_to_json(ctx, &libxl_nicinfo_val);
    printf("%s: %s\n", "libxl_nicinfo", s);
    if (s == NULL) abort();
    rc = libxl_nicinfo_from_json(ctx, &libxl_nicinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_nicinfo_to_json(ctx, &libxl_nicinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_nicinfo_dispose(&libxl_nicinfo_val);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);

    libxl_vtpminfo_rand_init(&libxl_vtpminfo_val);
    libxl_vtpminfo_init(&libxl_vtpminfo_val_new);
    libxl_vtpminfo_init(&libxl_vtpminfo_val_new);
    s = libxl_vtpminfo_to_json(ctx, &libxl_vtpminfo_val);
    printf("%s: %s\n", "libxl_vtpminfo", s);
    if (s == NULL) abort();
    rc = libxl_vtpminfo_from_json(ctx, &libxl_vtpminfo_val_new, s);
    if (rc) abort();
    new_s = libxl_vtpminfo_to_json(ctx, &libxl_vtpminfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vtpminfo_dispose(&libxl_vtpminfo_val);
    libxl_vtpminfo_dispose(&libxl_vtpminfo_val_new);

    libxl_usbctrlinfo_rand_init(&libxl_usbctrlinfo_val);
    libxl_usbctrlinfo_init(&libxl_usbctrlinfo_val_new);
    libxl_usbctrlinfo_init(&libxl_usbctrlinfo_val_new);
    libxl_usbctrlinfo_init(&libxl_usbctrlinfo_val_new);
    s = libxl_usbctrlinfo_to_json(ctx, &libxl_usbctrlinfo_val);
    printf("%s: %s\n", "libxl_usbctrlinfo", s);
    if (s == NULL) abort();
    rc = libxl_usbctrlinfo_from_json(ctx, &libxl_usbctrlinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_usbctrlinfo_to_json(ctx, &libxl_usbctrlinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_usbctrlinfo_dispose(&libxl_usbctrlinfo_val);
    libxl_usbctrlinfo_dispose(&libxl_usbctrlinfo_val_new);
    libxl_usbctrlinfo_dispose(&libxl_usbctrlinfo_val_new);

    libxl_vcpuinfo_rand_init(&libxl_vcpuinfo_val);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    s = libxl_vcpuinfo_to_json(ctx, &libxl_vcpuinfo_val);
    printf("%s: %s\n", "libxl_vcpuinfo", s);
    if (s == NULL) abort();
    rc = libxl_vcpuinfo_from_json(ctx, &libxl_vcpuinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_vcpuinfo_to_json(ctx, &libxl_vcpuinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);

    libxl_physinfo_rand_init(&libxl_physinfo_val);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    s = libxl_physinfo_to_json(ctx, &libxl_physinfo_val);
    printf("%s: %s\n", "libxl_physinfo", s);
    if (s == NULL) abort();
    rc = libxl_physinfo_from_json(ctx, &libxl_physinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_physinfo_to_json(ctx, &libxl_physinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_physinfo_dispose(&libxl_physinfo_val);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);

    libxl_connectorinfo_rand_init(&libxl_connectorinfo_val);
    libxl_connectorinfo_init(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_init(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_init(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_init(&libxl_connectorinfo_val_new);
    s = libxl_connectorinfo_to_json(ctx, &libxl_connectorinfo_val);
    printf("%s: %s\n", "libxl_connectorinfo", s);
    if (s == NULL) abort();
    rc = libxl_connectorinfo_from_json(ctx, &libxl_connectorinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_connectorinfo_to_json(ctx, &libxl_connectorinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);

    libxl_vdisplinfo_rand_init(&libxl_vdisplinfo_val);
    libxl_vdisplinfo_init(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_init(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_init(&libxl_vdisplinfo_val_new);
    s = libxl_vdisplinfo_to_json(ctx, &libxl_vdisplinfo_val);
    printf("%s: %s\n", "libxl_vdisplinfo", s);
    if (s == NULL) abort();
    rc = libxl_vdisplinfo_from_json(ctx, &libxl_vdisplinfo_val_new, s);
    if (rc) abort();
    new_s = libxl_vdisplinfo_to_json(ctx, &libxl_vdisplinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);

    libxl_numainfo_rand_init(&libxl_numainfo_val);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    s = libxl_numainfo_to_json(ctx, &libxl_numainfo_val);
    printf("%s: %s\n", "libxl_numainfo", s);
    if (s == NULL) abort();
    rc = libxl_numainfo_from_json(ctx, &libxl_numainfo_val_new, s);
    if (rc) abort();
    new_s = libxl_numainfo_to_json(ctx, &libxl_numainfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_numainfo_dispose(&libxl_numainfo_val);
    libxl_numainfo_dispose(&libxl_numainfo_val_new);
    libxl_numainfo_dispose(&libxl_numainfo_val_new);
    libxl_numainfo_dispose(&libxl_numainfo_val_new);
    libxl_numainfo_dispose(&libxl_numainfo_val_new);

    libxl_cputopology_rand_init(&libxl_cputopology_val);
    libxl_cputopology_init(&libxl_cputopology_val_new);
    libxl_cputopology_init(&libxl_cputopology_val_new);
    libxl_cputopology_init(&libxl_cputopology_val_new);
    libxl_cputopology_init(&libxl_cputopology_val_new);
    s = libxl_cputopology_to_json(ctx, &libxl_cputopology_val);
    printf("%s: %s\n", "libxl_cputopology", s);
    if (s == NULL) abort();
    rc = libxl_cputopology_from_json(ctx, &libxl_cputopology_val_new, s);
    if (rc) abort();
    new_s = libxl_cputopology_to_json(ctx, &libxl_cputopology_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_cputopology_dispose(&libxl_cputopology_val);
    libxl_cputopology_dispose(&libxl_cputopology_val_new);
    libxl_cputopology_dispose(&libxl_cputopology_val_new);
    libxl_cputopology_dispose(&libxl_cputopology_val_new);

    libxl_pcitopology_rand_init(&libxl_pcitopology_val);
    libxl_pcitopology_init(&libxl_pcitopology_val_new);
    libxl_pcitopology_init(&libxl_pcitopology_val_new);
    libxl_pcitopology_init(&libxl_pcitopology_val_new);
    libxl_pcitopology_init(&libxl_pcitopology_val_new);
    s = libxl_pcitopology_to_json(ctx, &libxl_pcitopology_val);
    printf("%s: %s\n", "libxl_pcitopology", s);
    if (s == NULL) abort();
    rc = libxl_pcitopology_from_json(ctx, &libxl_pcitopology_val_new, s);
    if (rc) abort();
    new_s = libxl_pcitopology_to_json(ctx, &libxl_pcitopology_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_pcitopology_dispose(&libxl_pcitopology_val);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);

    libxl_sched_credit_params_rand_init(&libxl_sched_credit_params_val);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    s = libxl_sched_credit_params_to_json(ctx, &libxl_sched_credit_params_val);
    printf("%s: %s\n", "libxl_sched_credit_params", s);
    if (s == NULL) abort();
    rc = libxl_sched_credit_params_from_json(ctx, &libxl_sched_credit_params_val_new, s);
    if (rc) abort();
    new_s = libxl_sched_credit_params_to_json(ctx, &libxl_sched_credit_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_sched_credit2_params_rand_init(&libxl_sched_credit2_params_val);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    s = libxl_sched_credit2_params_to_json(ctx, &libxl_sched_credit2_params_val);
    printf("%s: %s\n", "libxl_sched_credit2_params", s);
    if (s == NULL) abort();
    rc = libxl_sched_credit2_params_from_json(ctx, &libxl_sched_credit2_params_val_new, s);
    if (rc) abort();
    new_s = libxl_sched_credit2_params_to_json(ctx, &libxl_sched_credit2_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_domain_remus_info_rand_init(&libxl_domain_remus_info_val);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    s = libxl_domain_remus_info_to_json(ctx, &libxl_domain_remus_info_val);
    printf("%s: %s\n", "libxl_domain_remus_info", s);
    if (s == NULL) abort();
    rc = libxl_domain_remus_info_from_json(ctx, &libxl_domain_remus_info_val_new, s);
    if (rc) abort();
    new_s = libxl_domain_remus_info_to_json(ctx, &libxl_domain_remus_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val_new);

    libxl_event_type_rand_init(&libxl_event_type_val);
    s = libxl_event_type_to_json(ctx, libxl_event_type_val);
    printf("%s: %s\n", "libxl_event_type", s);
    if (s == NULL) abort();
    rc = libxl_event_type_from_json(ctx, &libxl_event_type_val_new, s);
    if (rc) abort();
    new_s = libxl_event_type_to_json(ctx, libxl_event_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_event_rand_init(&libxl_event_val);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_init(&libxl_event_val_new);
    s = libxl_event_to_json(ctx, &libxl_event_val);
    printf("%s: %s\n", "libxl_event", s);
    if (s == NULL) abort();
    rc = libxl_event_from_json(ctx, &libxl_event_val_new, s);
    if (rc) abort();
    new_s = libxl_event_to_json(ctx, &libxl_event_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_event_dispose(&libxl_event_val);
    libxl_event_dispose(&libxl_event_val_new);
    libxl_event_dispose(&libxl_event_val_new);
    libxl_event_dispose(&libxl_event_val_new);
    libxl_event_dispose(&libxl_event_val_new);
    libxl_event_dispose(&libxl_event_val_new);

    libxl_psr_cmt_type_rand_init(&libxl_psr_cmt_type_val);
    s = libxl_psr_cmt_type_to_json(ctx, libxl_psr_cmt_type_val);
    printf("%s: %s\n", "libxl_psr_cmt_type", s);
    if (s == NULL) abort();
    rc = libxl_psr_cmt_type_from_json(ctx, &libxl_psr_cmt_type_val_new, s);
    if (rc) abort();
    new_s = libxl_psr_cmt_type_to_json(ctx, libxl_psr_cmt_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_psr_cbm_type_rand_init(&libxl_psr_cbm_type_val);
    s = libxl_psr_cbm_type_to_json(ctx, libxl_psr_cbm_type_val);
    printf("%s: %s\n", "libxl_psr_cbm_type", s);
    if (s == NULL) abort();
    rc = libxl_psr_cbm_type_from_json(ctx, &libxl_psr_cbm_type_val_new, s);
    if (rc) abort();
    new_s = libxl_psr_cbm_type_to_json(ctx, libxl_psr_cbm_type_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);

    libxl_psr_cat_info_rand_init(&libxl_psr_cat_info_val);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    s = libxl_psr_cat_info_to_json(ctx, &libxl_psr_cat_info_val);
    printf("%s: %s\n", "libxl_psr_cat_info", s);
    if (s == NULL) abort();
    rc = libxl_psr_cat_info_from_json(ctx, &libxl_psr_cat_info_val_new, s);
    if (rc) abort();
    new_s = libxl_psr_cat_info_to_json(ctx, &libxl_psr_cat_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Regenerated string different from original string.\n");
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);

    printf("Testing TYPE_copy()\n");
    printf("----------------------\n");
    printf("\n");
    printf("Testing libxl_ioport_range_copy, ");
    libxl_ioport_range_init(&libxl_ioport_range_val);
    libxl_ioport_range_rand_init(&libxl_ioport_range_val);
    libxl_ioport_range_init(&libxl_ioport_range_val_new);
    libxl_ioport_range_copy(ctx, &libxl_ioport_range_val_new, &libxl_ioport_range_val);
    s = libxl_ioport_range_to_json(ctx, &libxl_ioport_range_val);
    if (s == NULL) abort();
    new_s = libxl_ioport_range_to_json(ctx, &libxl_ioport_range_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_ioport_range failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_ioport_range_dispose(&libxl_ioport_range_val);
    libxl_ioport_range_dispose(&libxl_ioport_range_val_new);
    printf("done\n");

    printf("Testing libxl_iomem_range_copy, ");
    libxl_iomem_range_init(&libxl_iomem_range_val);
    libxl_iomem_range_rand_init(&libxl_iomem_range_val);
    libxl_iomem_range_init(&libxl_iomem_range_val_new);
    libxl_iomem_range_copy(ctx, &libxl_iomem_range_val_new, &libxl_iomem_range_val);
    s = libxl_iomem_range_to_json(ctx, &libxl_iomem_range_val);
    if (s == NULL) abort();
    new_s = libxl_iomem_range_to_json(ctx, &libxl_iomem_range_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_iomem_range failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_iomem_range_dispose(&libxl_iomem_range_val);
    libxl_iomem_range_dispose(&libxl_iomem_range_val_new);
    printf("done\n");

    printf("Testing libxl_vga_interface_info_copy, ");
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val);
    libxl_vga_interface_info_rand_init(&libxl_vga_interface_info_val);
    libxl_vga_interface_info_init(&libxl_vga_interface_info_val_new);
    libxl_vga_interface_info_copy(ctx, &libxl_vga_interface_info_val_new, &libxl_vga_interface_info_val);
    s = libxl_vga_interface_info_to_json(ctx, &libxl_vga_interface_info_val);
    if (s == NULL) abort();
    new_s = libxl_vga_interface_info_to_json(ctx, &libxl_vga_interface_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vga_interface_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val);
    libxl_vga_interface_info_dispose(&libxl_vga_interface_info_val_new);
    printf("done\n");

    printf("Testing libxl_vnc_info_copy, ");
    libxl_vnc_info_init(&libxl_vnc_info_val);
    libxl_vnc_info_rand_init(&libxl_vnc_info_val);
    libxl_vnc_info_init(&libxl_vnc_info_val_new);
    libxl_vnc_info_copy(ctx, &libxl_vnc_info_val_new, &libxl_vnc_info_val);
    s = libxl_vnc_info_to_json(ctx, &libxl_vnc_info_val);
    if (s == NULL) abort();
    new_s = libxl_vnc_info_to_json(ctx, &libxl_vnc_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vnc_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vnc_info_dispose(&libxl_vnc_info_val);
    libxl_vnc_info_dispose(&libxl_vnc_info_val_new);
    printf("done\n");

    printf("Testing libxl_spice_info_copy, ");
    libxl_spice_info_init(&libxl_spice_info_val);
    libxl_spice_info_rand_init(&libxl_spice_info_val);
    libxl_spice_info_init(&libxl_spice_info_val_new);
    libxl_spice_info_copy(ctx, &libxl_spice_info_val_new, &libxl_spice_info_val);
    s = libxl_spice_info_to_json(ctx, &libxl_spice_info_val);
    if (s == NULL) abort();
    new_s = libxl_spice_info_to_json(ctx, &libxl_spice_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_spice_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_spice_info_dispose(&libxl_spice_info_val);
    libxl_spice_info_dispose(&libxl_spice_info_val_new);
    printf("done\n");

    printf("Testing libxl_sdl_info_copy, ");
    libxl_sdl_info_init(&libxl_sdl_info_val);
    libxl_sdl_info_rand_init(&libxl_sdl_info_val);
    libxl_sdl_info_init(&libxl_sdl_info_val_new);
    libxl_sdl_info_copy(ctx, &libxl_sdl_info_val_new, &libxl_sdl_info_val);
    s = libxl_sdl_info_to_json(ctx, &libxl_sdl_info_val);
    if (s == NULL) abort();
    new_s = libxl_sdl_info_to_json(ctx, &libxl_sdl_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_sdl_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_sdl_info_dispose(&libxl_sdl_info_val);
    libxl_sdl_info_dispose(&libxl_sdl_info_val_new);
    printf("done\n");

    printf("Testing libxl_dominfo_copy, ");
    libxl_dominfo_init(&libxl_dominfo_val);
    libxl_dominfo_rand_init(&libxl_dominfo_val);
    libxl_dominfo_init(&libxl_dominfo_val_new);
    libxl_dominfo_copy(ctx, &libxl_dominfo_val_new, &libxl_dominfo_val);
    s = libxl_dominfo_to_json(ctx, &libxl_dominfo_val);
    if (s == NULL) abort();
    new_s = libxl_dominfo_to_json(ctx, &libxl_dominfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_dominfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_dominfo_dispose(&libxl_dominfo_val);
    libxl_dominfo_dispose(&libxl_dominfo_val_new);
    printf("done\n");

    printf("Testing libxl_cpupoolinfo_copy, ");
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val);
    libxl_cpupoolinfo_rand_init(&libxl_cpupoolinfo_val);
    libxl_cpupoolinfo_init(&libxl_cpupoolinfo_val_new);
    libxl_cpupoolinfo_copy(ctx, &libxl_cpupoolinfo_val_new, &libxl_cpupoolinfo_val);
    s = libxl_cpupoolinfo_to_json(ctx, &libxl_cpupoolinfo_val);
    if (s == NULL) abort();
    new_s = libxl_cpupoolinfo_to_json(ctx, &libxl_cpupoolinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_cpupoolinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val);
    libxl_cpupoolinfo_dispose(&libxl_cpupoolinfo_val_new);
    printf("done\n");

    printf("Testing libxl_channelinfo_copy, ");
    libxl_channelinfo_init(&libxl_channelinfo_val);
    libxl_channelinfo_rand_init(&libxl_channelinfo_val);
    libxl_channelinfo_init(&libxl_channelinfo_val_new);
    libxl_channelinfo_copy(ctx, &libxl_channelinfo_val_new, &libxl_channelinfo_val);
    s = libxl_channelinfo_to_json(ctx, &libxl_channelinfo_val);
    if (s == NULL) abort();
    new_s = libxl_channelinfo_to_json(ctx, &libxl_channelinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_channelinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_channelinfo_dispose(&libxl_channelinfo_val);
    libxl_channelinfo_dispose(&libxl_channelinfo_val_new);
    printf("done\n");

    printf("Testing libxl_vminfo_copy, ");
    libxl_vminfo_init(&libxl_vminfo_val);
    libxl_vminfo_rand_init(&libxl_vminfo_val);
    libxl_vminfo_init(&libxl_vminfo_val_new);
    libxl_vminfo_copy(ctx, &libxl_vminfo_val_new, &libxl_vminfo_val);
    s = libxl_vminfo_to_json(ctx, &libxl_vminfo_val);
    if (s == NULL) abort();
    new_s = libxl_vminfo_to_json(ctx, &libxl_vminfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vminfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vminfo_dispose(&libxl_vminfo_val);
    libxl_vminfo_dispose(&libxl_vminfo_val_new);
    printf("done\n");

    printf("Testing libxl_version_info_copy, ");
    libxl_version_info_init(&libxl_version_info_val);
    libxl_version_info_rand_init(&libxl_version_info_val);
    libxl_version_info_init(&libxl_version_info_val_new);
    libxl_version_info_copy(ctx, &libxl_version_info_val_new, &libxl_version_info_val);
    s = libxl_version_info_to_json(ctx, &libxl_version_info_val);
    if (s == NULL) abort();
    new_s = libxl_version_info_to_json(ctx, &libxl_version_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_version_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_version_info_dispose(&libxl_version_info_val);
    libxl_version_info_dispose(&libxl_version_info_val_new);
    printf("done\n");

    printf("Testing libxl_domain_create_info_copy, ");
    libxl_domain_create_info_init(&libxl_domain_create_info_val);
    libxl_domain_create_info_rand_init(&libxl_domain_create_info_val);
    libxl_domain_create_info_init(&libxl_domain_create_info_val_new);
    libxl_domain_create_info_copy(ctx, &libxl_domain_create_info_val_new, &libxl_domain_create_info_val);
    s = libxl_domain_create_info_to_json(ctx, &libxl_domain_create_info_val);
    if (s == NULL) abort();
    new_s = libxl_domain_create_info_to_json(ctx, &libxl_domain_create_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_create_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val);
    libxl_domain_create_info_dispose(&libxl_domain_create_info_val_new);
    printf("done\n");

    printf("Testing libxl_domain_restore_params_copy, ");
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val);
    libxl_domain_restore_params_rand_init(&libxl_domain_restore_params_val);
    libxl_domain_restore_params_init(&libxl_domain_restore_params_val_new);
    libxl_domain_restore_params_copy(ctx, &libxl_domain_restore_params_val_new, &libxl_domain_restore_params_val);
    s = libxl_domain_restore_params_to_json(ctx, &libxl_domain_restore_params_val);
    if (s == NULL) abort();
    new_s = libxl_domain_restore_params_to_json(ctx, &libxl_domain_restore_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_restore_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val);
    libxl_domain_restore_params_dispose(&libxl_domain_restore_params_val_new);
    printf("done\n");

    printf("Testing libxl_sched_params_copy, ");
    libxl_sched_params_init(&libxl_sched_params_val);
    libxl_sched_params_rand_init(&libxl_sched_params_val);
    libxl_sched_params_init(&libxl_sched_params_val_new);
    libxl_sched_params_copy(ctx, &libxl_sched_params_val_new, &libxl_sched_params_val);
    s = libxl_sched_params_to_json(ctx, &libxl_sched_params_val);
    if (s == NULL) abort();
    new_s = libxl_sched_params_to_json(ctx, &libxl_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_sched_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_sched_params_dispose(&libxl_sched_params_val);
    libxl_sched_params_dispose(&libxl_sched_params_val_new);
    printf("done\n");

    printf("Testing libxl_vcpu_sched_params_copy, ");
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val);
    libxl_vcpu_sched_params_rand_init(&libxl_vcpu_sched_params_val);
    libxl_vcpu_sched_params_init(&libxl_vcpu_sched_params_val_new);
    libxl_vcpu_sched_params_copy(ctx, &libxl_vcpu_sched_params_val_new, &libxl_vcpu_sched_params_val);
    s = libxl_vcpu_sched_params_to_json(ctx, &libxl_vcpu_sched_params_val);
    if (s == NULL) abort();
    new_s = libxl_vcpu_sched_params_to_json(ctx, &libxl_vcpu_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vcpu_sched_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vcpu_sched_params_dispose(&libxl_vcpu_sched_params_val);
    libxl_vcpu_sched_params_dispose(&libxl_vcpu_sched_params_val_new);
    printf("done\n");

    printf("Testing libxl_domain_sched_params_copy, ");
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val);
    libxl_domain_sched_params_rand_init(&libxl_domain_sched_params_val);
    libxl_domain_sched_params_init(&libxl_domain_sched_params_val_new);
    libxl_domain_sched_params_copy(ctx, &libxl_domain_sched_params_val_new, &libxl_domain_sched_params_val);
    s = libxl_domain_sched_params_to_json(ctx, &libxl_domain_sched_params_val);
    if (s == NULL) abort();
    new_s = libxl_domain_sched_params_to_json(ctx, &libxl_domain_sched_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_sched_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val);
    libxl_domain_sched_params_dispose(&libxl_domain_sched_params_val_new);
    printf("done\n");

    printf("Testing libxl_vnode_info_copy, ");
    libxl_vnode_info_init(&libxl_vnode_info_val);
    libxl_vnode_info_rand_init(&libxl_vnode_info_val);
    libxl_vnode_info_init(&libxl_vnode_info_val_new);
    libxl_vnode_info_copy(ctx, &libxl_vnode_info_val_new, &libxl_vnode_info_val);
    s = libxl_vnode_info_to_json(ctx, &libxl_vnode_info_val);
    if (s == NULL) abort();
    new_s = libxl_vnode_info_to_json(ctx, &libxl_vnode_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vnode_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vnode_info_dispose(&libxl_vnode_info_val);
    libxl_vnode_info_dispose(&libxl_vnode_info_val_new);
    printf("done\n");

    printf("Testing libxl_rdm_reserve_copy, ");
    libxl_rdm_reserve_init(&libxl_rdm_reserve_val);
    libxl_rdm_reserve_rand_init(&libxl_rdm_reserve_val);
    libxl_rdm_reserve_init(&libxl_rdm_reserve_val_new);
    libxl_rdm_reserve_copy(ctx, &libxl_rdm_reserve_val_new, &libxl_rdm_reserve_val);
    s = libxl_rdm_reserve_to_json(ctx, &libxl_rdm_reserve_val);
    if (s == NULL) abort();
    new_s = libxl_rdm_reserve_to_json(ctx, &libxl_rdm_reserve_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_rdm_reserve failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val);
    libxl_rdm_reserve_dispose(&libxl_rdm_reserve_val_new);
    printf("done\n");

    printf("Testing libxl_domain_build_info_copy, ");
    libxl_domain_build_info_init(&libxl_domain_build_info_val);
    libxl_domain_build_info_rand_init(&libxl_domain_build_info_val);
    libxl_domain_build_info_init(&libxl_domain_build_info_val_new);
    libxl_domain_build_info_copy(ctx, &libxl_domain_build_info_val_new, &libxl_domain_build_info_val);
    s = libxl_domain_build_info_to_json(ctx, &libxl_domain_build_info_val);
    if (s == NULL) abort();
    new_s = libxl_domain_build_info_to_json(ctx, &libxl_domain_build_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_build_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val);
    libxl_domain_build_info_dispose(&libxl_domain_build_info_val_new);
    printf("done\n");

    printf("Testing libxl_device_vfb_copy, ");
    libxl_device_vfb_init(&libxl_device_vfb_val);
    libxl_device_vfb_rand_init(&libxl_device_vfb_val);
    libxl_device_vfb_init(&libxl_device_vfb_val_new);
    libxl_device_vfb_copy(ctx, &libxl_device_vfb_val_new, &libxl_device_vfb_val);
    s = libxl_device_vfb_to_json(ctx, &libxl_device_vfb_val);
    if (s == NULL) abort();
    new_s = libxl_device_vfb_to_json(ctx, &libxl_device_vfb_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_vfb failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vfb_dispose(&libxl_device_vfb_val);
    libxl_device_vfb_dispose(&libxl_device_vfb_val_new);
    printf("done\n");

    printf("Testing libxl_device_vkb_copy, ");
    libxl_device_vkb_init(&libxl_device_vkb_val);
    libxl_device_vkb_rand_init(&libxl_device_vkb_val);
    libxl_device_vkb_init(&libxl_device_vkb_val_new);
    libxl_device_vkb_copy(ctx, &libxl_device_vkb_val_new, &libxl_device_vkb_val);
    s = libxl_device_vkb_to_json(ctx, &libxl_device_vkb_val);
    if (s == NULL) abort();
    new_s = libxl_device_vkb_to_json(ctx, &libxl_device_vkb_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_vkb failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vkb_dispose(&libxl_device_vkb_val);
    libxl_device_vkb_dispose(&libxl_device_vkb_val_new);
    printf("done\n");

    printf("Testing libxl_device_disk_copy, ");
    libxl_device_disk_init(&libxl_device_disk_val);
    libxl_device_disk_rand_init(&libxl_device_disk_val);
    libxl_device_disk_init(&libxl_device_disk_val_new);
    libxl_device_disk_copy(ctx, &libxl_device_disk_val_new, &libxl_device_disk_val);
    s = libxl_device_disk_to_json(ctx, &libxl_device_disk_val);
    if (s == NULL) abort();
    new_s = libxl_device_disk_to_json(ctx, &libxl_device_disk_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_disk failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_disk_dispose(&libxl_device_disk_val);
    libxl_device_disk_dispose(&libxl_device_disk_val_new);
    printf("done\n");

    printf("Testing libxl_device_nic_copy, ");
    libxl_device_nic_init(&libxl_device_nic_val);
    libxl_device_nic_rand_init(&libxl_device_nic_val);
    libxl_device_nic_init(&libxl_device_nic_val_new);
    libxl_device_nic_copy(ctx, &libxl_device_nic_val_new, &libxl_device_nic_val);
    s = libxl_device_nic_to_json(ctx, &libxl_device_nic_val);
    if (s == NULL) abort();
    new_s = libxl_device_nic_to_json(ctx, &libxl_device_nic_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_nic failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_nic_dispose(&libxl_device_nic_val);
    libxl_device_nic_dispose(&libxl_device_nic_val_new);
    printf("done\n");

    printf("Testing libxl_device_pci_copy, ");
    libxl_device_pci_init(&libxl_device_pci_val);
    libxl_device_pci_rand_init(&libxl_device_pci_val);
    libxl_device_pci_init(&libxl_device_pci_val_new);
    libxl_device_pci_copy(ctx, &libxl_device_pci_val_new, &libxl_device_pci_val);
    s = libxl_device_pci_to_json(ctx, &libxl_device_pci_val);
    if (s == NULL) abort();
    new_s = libxl_device_pci_to_json(ctx, &libxl_device_pci_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_pci failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_pci_dispose(&libxl_device_pci_val);
    libxl_device_pci_dispose(&libxl_device_pci_val_new);
    printf("done\n");

    printf("Testing libxl_device_rdm_copy, ");
    libxl_device_rdm_init(&libxl_device_rdm_val);
    libxl_device_rdm_rand_init(&libxl_device_rdm_val);
    libxl_device_rdm_init(&libxl_device_rdm_val_new);
    libxl_device_rdm_copy(ctx, &libxl_device_rdm_val_new, &libxl_device_rdm_val);
    s = libxl_device_rdm_to_json(ctx, &libxl_device_rdm_val);
    if (s == NULL) abort();
    new_s = libxl_device_rdm_to_json(ctx, &libxl_device_rdm_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_rdm failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_rdm_dispose(&libxl_device_rdm_val);
    libxl_device_rdm_dispose(&libxl_device_rdm_val_new);
    printf("done\n");

    printf("Testing libxl_device_usbctrl_copy, ");
    libxl_device_usbctrl_init(&libxl_device_usbctrl_val);
    libxl_device_usbctrl_rand_init(&libxl_device_usbctrl_val);
    libxl_device_usbctrl_init(&libxl_device_usbctrl_val_new);
    libxl_device_usbctrl_copy(ctx, &libxl_device_usbctrl_val_new, &libxl_device_usbctrl_val);
    s = libxl_device_usbctrl_to_json(ctx, &libxl_device_usbctrl_val);
    if (s == NULL) abort();
    new_s = libxl_device_usbctrl_to_json(ctx, &libxl_device_usbctrl_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_usbctrl failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val);
    libxl_device_usbctrl_dispose(&libxl_device_usbctrl_val_new);
    printf("done\n");

    printf("Testing libxl_device_usbdev_copy, ");
    libxl_device_usbdev_init(&libxl_device_usbdev_val);
    libxl_device_usbdev_rand_init(&libxl_device_usbdev_val);
    libxl_device_usbdev_init(&libxl_device_usbdev_val_new);
    libxl_device_usbdev_copy(ctx, &libxl_device_usbdev_val_new, &libxl_device_usbdev_val);
    s = libxl_device_usbdev_to_json(ctx, &libxl_device_usbdev_val);
    if (s == NULL) abort();
    new_s = libxl_device_usbdev_to_json(ctx, &libxl_device_usbdev_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_usbdev failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_usbdev_dispose(&libxl_device_usbdev_val);
    libxl_device_usbdev_dispose(&libxl_device_usbdev_val_new);
    printf("done\n");

    printf("Testing libxl_device_dtdev_copy, ");
    libxl_device_dtdev_init(&libxl_device_dtdev_val);
    libxl_device_dtdev_rand_init(&libxl_device_dtdev_val);
    libxl_device_dtdev_init(&libxl_device_dtdev_val_new);
    libxl_device_dtdev_copy(ctx, &libxl_device_dtdev_val_new, &libxl_device_dtdev_val);
    s = libxl_device_dtdev_to_json(ctx, &libxl_device_dtdev_val);
    if (s == NULL) abort();
    new_s = libxl_device_dtdev_to_json(ctx, &libxl_device_dtdev_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_dtdev failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val);
    libxl_device_dtdev_dispose(&libxl_device_dtdev_val_new);
    printf("done\n");

    printf("Testing libxl_device_vtpm_copy, ");
    libxl_device_vtpm_init(&libxl_device_vtpm_val);
    libxl_device_vtpm_rand_init(&libxl_device_vtpm_val);
    libxl_device_vtpm_init(&libxl_device_vtpm_val_new);
    libxl_device_vtpm_copy(ctx, &libxl_device_vtpm_val_new, &libxl_device_vtpm_val);
    s = libxl_device_vtpm_to_json(ctx, &libxl_device_vtpm_val);
    if (s == NULL) abort();
    new_s = libxl_device_vtpm_to_json(ctx, &libxl_device_vtpm_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_vtpm failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val);
    libxl_device_vtpm_dispose(&libxl_device_vtpm_val_new);
    printf("done\n");

    printf("Testing libxl_device_p9_copy, ");
    libxl_device_p9_init(&libxl_device_p9_val);
    libxl_device_p9_rand_init(&libxl_device_p9_val);
    libxl_device_p9_init(&libxl_device_p9_val_new);
    libxl_device_p9_copy(ctx, &libxl_device_p9_val_new, &libxl_device_p9_val);
    s = libxl_device_p9_to_json(ctx, &libxl_device_p9_val);
    if (s == NULL) abort();
    new_s = libxl_device_p9_to_json(ctx, &libxl_device_p9_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_p9 failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_p9_dispose(&libxl_device_p9_val);
    libxl_device_p9_dispose(&libxl_device_p9_val_new);
    printf("done\n");

    printf("Testing libxl_device_channel_copy, ");
    libxl_device_channel_init(&libxl_device_channel_val);
    libxl_device_channel_rand_init(&libxl_device_channel_val);
    libxl_device_channel_init(&libxl_device_channel_val_new);
    libxl_device_channel_copy(ctx, &libxl_device_channel_val_new, &libxl_device_channel_val);
    s = libxl_device_channel_to_json(ctx, &libxl_device_channel_val);
    if (s == NULL) abort();
    new_s = libxl_device_channel_to_json(ctx, &libxl_device_channel_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_channel failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_channel_dispose(&libxl_device_channel_val);
    libxl_device_channel_dispose(&libxl_device_channel_val_new);
    printf("done\n");

    printf("Testing libxl_connector_param_copy, ");
    libxl_connector_param_init(&libxl_connector_param_val);
    libxl_connector_param_rand_init(&libxl_connector_param_val);
    libxl_connector_param_init(&libxl_connector_param_val_new);
    libxl_connector_param_copy(ctx, &libxl_connector_param_val_new, &libxl_connector_param_val);
    s = libxl_connector_param_to_json(ctx, &libxl_connector_param_val);
    if (s == NULL) abort();
    new_s = libxl_connector_param_to_json(ctx, &libxl_connector_param_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_connector_param failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_connector_param_dispose(&libxl_connector_param_val);
    libxl_connector_param_dispose(&libxl_connector_param_val_new);
    printf("done\n");

    printf("Testing libxl_device_vdispl_copy, ");
    libxl_device_vdispl_init(&libxl_device_vdispl_val);
    libxl_device_vdispl_rand_init(&libxl_device_vdispl_val);
    libxl_device_vdispl_init(&libxl_device_vdispl_val_new);
    libxl_device_vdispl_copy(ctx, &libxl_device_vdispl_val_new, &libxl_device_vdispl_val);
    s = libxl_device_vdispl_to_json(ctx, &libxl_device_vdispl_val);
    if (s == NULL) abort();
    new_s = libxl_device_vdispl_to_json(ctx, &libxl_device_vdispl_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_device_vdispl failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val);
    libxl_device_vdispl_dispose(&libxl_device_vdispl_val_new);
    printf("done\n");

    printf("Testing libxl_domain_config_copy, ");
    libxl_domain_config_init(&libxl_domain_config_val);
    libxl_domain_config_rand_init(&libxl_domain_config_val);
    libxl_domain_config_init(&libxl_domain_config_val_new);
    libxl_domain_config_copy(ctx, &libxl_domain_config_val_new, &libxl_domain_config_val);
    s = libxl_domain_config_to_json(ctx, &libxl_domain_config_val);
    if (s == NULL) abort();
    new_s = libxl_domain_config_to_json(ctx, &libxl_domain_config_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_config failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_config_dispose(&libxl_domain_config_val);
    libxl_domain_config_dispose(&libxl_domain_config_val_new);
    printf("done\n");

    printf("Testing libxl_diskinfo_copy, ");
    libxl_diskinfo_init(&libxl_diskinfo_val);
    libxl_diskinfo_rand_init(&libxl_diskinfo_val);
    libxl_diskinfo_init(&libxl_diskinfo_val_new);
    libxl_diskinfo_copy(ctx, &libxl_diskinfo_val_new, &libxl_diskinfo_val);
    s = libxl_diskinfo_to_json(ctx, &libxl_diskinfo_val);
    if (s == NULL) abort();
    new_s = libxl_diskinfo_to_json(ctx, &libxl_diskinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_diskinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_diskinfo_dispose(&libxl_diskinfo_val);
    libxl_diskinfo_dispose(&libxl_diskinfo_val_new);
    printf("done\n");

    printf("Testing libxl_nicinfo_copy, ");
    libxl_nicinfo_init(&libxl_nicinfo_val);
    libxl_nicinfo_rand_init(&libxl_nicinfo_val);
    libxl_nicinfo_init(&libxl_nicinfo_val_new);
    libxl_nicinfo_copy(ctx, &libxl_nicinfo_val_new, &libxl_nicinfo_val);
    s = libxl_nicinfo_to_json(ctx, &libxl_nicinfo_val);
    if (s == NULL) abort();
    new_s = libxl_nicinfo_to_json(ctx, &libxl_nicinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_nicinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_nicinfo_dispose(&libxl_nicinfo_val);
    libxl_nicinfo_dispose(&libxl_nicinfo_val_new);
    printf("done\n");

    printf("Testing libxl_vtpminfo_copy, ");
    libxl_vtpminfo_init(&libxl_vtpminfo_val);
    libxl_vtpminfo_rand_init(&libxl_vtpminfo_val);
    libxl_vtpminfo_init(&libxl_vtpminfo_val_new);
    libxl_vtpminfo_copy(ctx, &libxl_vtpminfo_val_new, &libxl_vtpminfo_val);
    s = libxl_vtpminfo_to_json(ctx, &libxl_vtpminfo_val);
    if (s == NULL) abort();
    new_s = libxl_vtpminfo_to_json(ctx, &libxl_vtpminfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vtpminfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vtpminfo_dispose(&libxl_vtpminfo_val);
    libxl_vtpminfo_dispose(&libxl_vtpminfo_val_new);
    printf("done\n");

    printf("Testing libxl_usbctrlinfo_copy, ");
    libxl_usbctrlinfo_init(&libxl_usbctrlinfo_val);
    libxl_usbctrlinfo_rand_init(&libxl_usbctrlinfo_val);
    libxl_usbctrlinfo_init(&libxl_usbctrlinfo_val_new);
    libxl_usbctrlinfo_copy(ctx, &libxl_usbctrlinfo_val_new, &libxl_usbctrlinfo_val);
    s = libxl_usbctrlinfo_to_json(ctx, &libxl_usbctrlinfo_val);
    if (s == NULL) abort();
    new_s = libxl_usbctrlinfo_to_json(ctx, &libxl_usbctrlinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_usbctrlinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_usbctrlinfo_dispose(&libxl_usbctrlinfo_val);
    libxl_usbctrlinfo_dispose(&libxl_usbctrlinfo_val_new);
    printf("done\n");

    printf("Testing libxl_vcpuinfo_copy, ");
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val);
    libxl_vcpuinfo_rand_init(&libxl_vcpuinfo_val);
    libxl_vcpuinfo_init(&libxl_vcpuinfo_val_new);
    libxl_vcpuinfo_copy(ctx, &libxl_vcpuinfo_val_new, &libxl_vcpuinfo_val);
    s = libxl_vcpuinfo_to_json(ctx, &libxl_vcpuinfo_val);
    if (s == NULL) abort();
    new_s = libxl_vcpuinfo_to_json(ctx, &libxl_vcpuinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vcpuinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val);
    libxl_vcpuinfo_dispose(&libxl_vcpuinfo_val_new);
    printf("done\n");

    printf("Testing libxl_physinfo_copy, ");
    libxl_physinfo_init(&libxl_physinfo_val);
    libxl_physinfo_rand_init(&libxl_physinfo_val);
    libxl_physinfo_init(&libxl_physinfo_val_new);
    libxl_physinfo_copy(ctx, &libxl_physinfo_val_new, &libxl_physinfo_val);
    s = libxl_physinfo_to_json(ctx, &libxl_physinfo_val);
    if (s == NULL) abort();
    new_s = libxl_physinfo_to_json(ctx, &libxl_physinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_physinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_physinfo_dispose(&libxl_physinfo_val);
    libxl_physinfo_dispose(&libxl_physinfo_val_new);
    printf("done\n");

    printf("Testing libxl_connectorinfo_copy, ");
    libxl_connectorinfo_init(&libxl_connectorinfo_val);
    libxl_connectorinfo_rand_init(&libxl_connectorinfo_val);
    libxl_connectorinfo_init(&libxl_connectorinfo_val_new);
    libxl_connectorinfo_copy(ctx, &libxl_connectorinfo_val_new, &libxl_connectorinfo_val);
    s = libxl_connectorinfo_to_json(ctx, &libxl_connectorinfo_val);
    if (s == NULL) abort();
    new_s = libxl_connectorinfo_to_json(ctx, &libxl_connectorinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_connectorinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val);
    libxl_connectorinfo_dispose(&libxl_connectorinfo_val_new);
    printf("done\n");

    printf("Testing libxl_vdisplinfo_copy, ");
    libxl_vdisplinfo_init(&libxl_vdisplinfo_val);
    libxl_vdisplinfo_rand_init(&libxl_vdisplinfo_val);
    libxl_vdisplinfo_init(&libxl_vdisplinfo_val_new);
    libxl_vdisplinfo_copy(ctx, &libxl_vdisplinfo_val_new, &libxl_vdisplinfo_val);
    s = libxl_vdisplinfo_to_json(ctx, &libxl_vdisplinfo_val);
    if (s == NULL) abort();
    new_s = libxl_vdisplinfo_to_json(ctx, &libxl_vdisplinfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_vdisplinfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val);
    libxl_vdisplinfo_dispose(&libxl_vdisplinfo_val_new);
    printf("done\n");

    printf("Testing libxl_numainfo_copy, ");
    libxl_numainfo_init(&libxl_numainfo_val);
    libxl_numainfo_rand_init(&libxl_numainfo_val);
    libxl_numainfo_init(&libxl_numainfo_val_new);
    libxl_numainfo_copy(ctx, &libxl_numainfo_val_new, &libxl_numainfo_val);
    s = libxl_numainfo_to_json(ctx, &libxl_numainfo_val);
    if (s == NULL) abort();
    new_s = libxl_numainfo_to_json(ctx, &libxl_numainfo_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_numainfo failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_numainfo_dispose(&libxl_numainfo_val);
    libxl_numainfo_dispose(&libxl_numainfo_val_new);
    printf("done\n");

    printf("Testing libxl_cputopology_copy, ");
    libxl_cputopology_init(&libxl_cputopology_val);
    libxl_cputopology_rand_init(&libxl_cputopology_val);
    libxl_cputopology_init(&libxl_cputopology_val_new);
    libxl_cputopology_copy(ctx, &libxl_cputopology_val_new, &libxl_cputopology_val);
    s = libxl_cputopology_to_json(ctx, &libxl_cputopology_val);
    if (s == NULL) abort();
    new_s = libxl_cputopology_to_json(ctx, &libxl_cputopology_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_cputopology failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_cputopology_dispose(&libxl_cputopology_val);
    libxl_cputopology_dispose(&libxl_cputopology_val_new);
    printf("done\n");

    printf("Testing libxl_pcitopology_copy, ");
    libxl_pcitopology_init(&libxl_pcitopology_val);
    libxl_pcitopology_rand_init(&libxl_pcitopology_val);
    libxl_pcitopology_init(&libxl_pcitopology_val_new);
    libxl_pcitopology_copy(ctx, &libxl_pcitopology_val_new, &libxl_pcitopology_val);
    s = libxl_pcitopology_to_json(ctx, &libxl_pcitopology_val);
    if (s == NULL) abort();
    new_s = libxl_pcitopology_to_json(ctx, &libxl_pcitopology_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_pcitopology failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_pcitopology_dispose(&libxl_pcitopology_val);
    libxl_pcitopology_dispose(&libxl_pcitopology_val_new);
    printf("done\n");

    printf("Testing libxl_sched_credit_params_copy, ");
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val);
    libxl_sched_credit_params_rand_init(&libxl_sched_credit_params_val);
    libxl_sched_credit_params_init(&libxl_sched_credit_params_val_new);
    libxl_sched_credit_params_copy(ctx, &libxl_sched_credit_params_val_new, &libxl_sched_credit_params_val);
    s = libxl_sched_credit_params_to_json(ctx, &libxl_sched_credit_params_val);
    if (s == NULL) abort();
    new_s = libxl_sched_credit_params_to_json(ctx, &libxl_sched_credit_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_sched_credit_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    printf("done\n");

    printf("Testing libxl_sched_credit2_params_copy, ");
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val);
    libxl_sched_credit2_params_rand_init(&libxl_sched_credit2_params_val);
    libxl_sched_credit2_params_init(&libxl_sched_credit2_params_val_new);
    libxl_sched_credit2_params_copy(ctx, &libxl_sched_credit2_params_val_new, &libxl_sched_credit2_params_val);
    s = libxl_sched_credit2_params_to_json(ctx, &libxl_sched_credit2_params_val);
    if (s == NULL) abort();
    new_s = libxl_sched_credit2_params_to_json(ctx, &libxl_sched_credit2_params_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_sched_credit2_params failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    printf("done\n");

    printf("Testing libxl_domain_remus_info_copy, ");
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val);
    libxl_domain_remus_info_rand_init(&libxl_domain_remus_info_val);
    libxl_domain_remus_info_init(&libxl_domain_remus_info_val_new);
    libxl_domain_remus_info_copy(ctx, &libxl_domain_remus_info_val_new, &libxl_domain_remus_info_val);
    s = libxl_domain_remus_info_to_json(ctx, &libxl_domain_remus_info_val);
    if (s == NULL) abort();
    new_s = libxl_domain_remus_info_to_json(ctx, &libxl_domain_remus_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_domain_remus_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val);
    libxl_domain_remus_info_dispose(&libxl_domain_remus_info_val_new);
    printf("done\n");

    printf("Testing libxl_event_copy, ");
    libxl_event_init(&libxl_event_val);
    libxl_event_rand_init(&libxl_event_val);
    libxl_event_init(&libxl_event_val_new);
    libxl_event_copy(ctx, &libxl_event_val_new, &libxl_event_val);
    s = libxl_event_to_json(ctx, &libxl_event_val);
    if (s == NULL) abort();
    new_s = libxl_event_to_json(ctx, &libxl_event_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_event failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_event_dispose(&libxl_event_val);
    libxl_event_dispose(&libxl_event_val_new);
    printf("done\n");

    printf("Testing libxl_psr_cat_info_copy, ");
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val);
    libxl_psr_cat_info_rand_init(&libxl_psr_cat_info_val);
    libxl_psr_cat_info_init(&libxl_psr_cat_info_val_new);
    libxl_psr_cat_info_copy(ctx, &libxl_psr_cat_info_val_new, &libxl_psr_cat_info_val);
    s = libxl_psr_cat_info_to_json(ctx, &libxl_psr_cat_info_val);
    if (s == NULL) abort();
    new_s = libxl_psr_cat_info_to_json(ctx, &libxl_psr_cat_info_val_new);
    if (new_s == NULL) abort();
    if (strcmp(s, new_s)) {
        printf("Huh? Deep copy for libxl_psr_cat_info failed. Regenerated string different from original string.\n");
        printf("Original string: %s\n", s);
        printf("Regenerated string: %s\n", new_s);
        abort();
    }
    free(s);
    free(new_s);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val);
    libxl_psr_cat_info_dispose(&libxl_psr_cat_info_val_new);
    printf("done\n");

    printf("\n");
    printf("Testing Enumerations\n");
    printf("--------------------\n");
    printf("\n");
    printf("libxl_domain_unpriv -- to string:\n");
    printf("\tDOM0 = %d = \"%s\"\n", LIBXL_DOMAIN_UNPRIV_DOM0, libxl_domain_unpriv_to_string(LIBXL_DOMAIN_UNPRIV_DOM0));
    printf("\tDOMU = %d = \"%s\"\n", LIBXL_DOMAIN_UNPRIV_DOMU, libxl_domain_unpriv_to_string(LIBXL_DOMAIN_UNPRIV_DOMU));

    printf("libxl_domain_unpriv -- to JSON:\n");
    json_string = libxl_domain_unpriv_to_json(ctx, LIBXL_DOMAIN_UNPRIV_DOM0);
    printf("\tDOM0 = %d = %s", LIBXL_DOMAIN_UNPRIV_DOM0, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_domain_unpriv_to_json(ctx, LIBXL_DOMAIN_UNPRIV_DOMU);
    printf("\tDOMU = %d = %s", LIBXL_DOMAIN_UNPRIV_DOMU, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_domain_unpriv -- from string:\n");
    libxl_domain_unpriv_val = -1;
    rc = libxl_domain_unpriv_from_string("Dom0", &libxl_domain_unpriv_val);
    printf("\tDOM0 = \"%s\" = %d (rc %d)\n", "Dom0", libxl_domain_unpriv_val, rc);
    libxl_domain_unpriv_val = -1;
    rc = libxl_domain_unpriv_from_string("doMU", &libxl_domain_unpriv_val);
    printf("\tDOMU = \"%s\" = %d (rc %d)\n", "doMU", libxl_domain_unpriv_val, rc);
    libxl_domain_unpriv_val = -1;
    rc = libxl_domain_unpriv_from_string("aN INvalId vAluE", &libxl_domain_unpriv_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN INvalId vAluE", libxl_domain_unpriv_val, rc);

    printf("libxl_error -- to string:\n");
    printf("\tNONSPECIFIC = %d = \"%s\"\n", ERROR_NONSPECIFIC, libxl_error_to_string(ERROR_NONSPECIFIC));
    printf("\tVERSION = %d = \"%s\"\n", ERROR_VERSION, libxl_error_to_string(ERROR_VERSION));
    printf("\tFAIL = %d = \"%s\"\n", ERROR_FAIL, libxl_error_to_string(ERROR_FAIL));
    printf("\tNI = %d = \"%s\"\n", ERROR_NI, libxl_error_to_string(ERROR_NI));
    printf("\tNOMEM = %d = \"%s\"\n", ERROR_NOMEM, libxl_error_to_string(ERROR_NOMEM));
    printf("\tINVAL = %d = \"%s\"\n", ERROR_INVAL, libxl_error_to_string(ERROR_INVAL));
    printf("\tBADFAIL = %d = \"%s\"\n", ERROR_BADFAIL, libxl_error_to_string(ERROR_BADFAIL));
    printf("\tGUEST_TIMEDOUT = %d = \"%s\"\n", ERROR_GUEST_TIMEDOUT, libxl_error_to_string(ERROR_GUEST_TIMEDOUT));
    printf("\tTIMEDOUT = %d = \"%s\"\n", ERROR_TIMEDOUT, libxl_error_to_string(ERROR_TIMEDOUT));
    printf("\tNOPARAVIRT = %d = \"%s\"\n", ERROR_NOPARAVIRT, libxl_error_to_string(ERROR_NOPARAVIRT));
    printf("\tNOT_READY = %d = \"%s\"\n", ERROR_NOT_READY, libxl_error_to_string(ERROR_NOT_READY));
    printf("\tOSEVENT_REG_FAIL = %d = \"%s\"\n", ERROR_OSEVENT_REG_FAIL, libxl_error_to_string(ERROR_OSEVENT_REG_FAIL));
    printf("\tBUFFERFULL = %d = \"%s\"\n", ERROR_BUFFERFULL, libxl_error_to_string(ERROR_BUFFERFULL));
    printf("\tUNKNOWN_CHILD = %d = \"%s\"\n", ERROR_UNKNOWN_CHILD, libxl_error_to_string(ERROR_UNKNOWN_CHILD));
    printf("\tLOCK_FAIL = %d = \"%s\"\n", ERROR_LOCK_FAIL, libxl_error_to_string(ERROR_LOCK_FAIL));
    printf("\tJSON_CONFIG_EMPTY = %d = \"%s\"\n", ERROR_JSON_CONFIG_EMPTY, libxl_error_to_string(ERROR_JSON_CONFIG_EMPTY));
    printf("\tDEVICE_EXISTS = %d = \"%s\"\n", ERROR_DEVICE_EXISTS, libxl_error_to_string(ERROR_DEVICE_EXISTS));
    printf("\tCHECKPOINT_DEVOPS_DOES_NOT_MATCH = %d = \"%s\"\n", ERROR_CHECKPOINT_DEVOPS_DOES_NOT_MATCH, libxl_error_to_string(ERROR_CHECKPOINT_DEVOPS_DOES_NOT_MATCH));
    printf("\tCHECKPOINT_DEVICE_NOT_SUPPORTED = %d = \"%s\"\n", ERROR_CHECKPOINT_DEVICE_NOT_SUPPORTED, libxl_error_to_string(ERROR_CHECKPOINT_DEVICE_NOT_SUPPORTED));
    printf("\tVNUMA_CONFIG_INVALID = %d = \"%s\"\n", ERROR_VNUMA_CONFIG_INVALID, libxl_error_to_string(ERROR_VNUMA_CONFIG_INVALID));
    printf("\tDOMAIN_NOTFOUND = %d = \"%s\"\n", ERROR_DOMAIN_NOTFOUND, libxl_error_to_string(ERROR_DOMAIN_NOTFOUND));
    printf("\tABORTED = %d = \"%s\"\n", ERROR_ABORTED, libxl_error_to_string(ERROR_ABORTED));
    printf("\tNOTFOUND = %d = \"%s\"\n", ERROR_NOTFOUND, libxl_error_to_string(ERROR_NOTFOUND));
    printf("\tDOMAIN_DESTROYED = %d = \"%s\"\n", ERROR_DOMAIN_DESTROYED, libxl_error_to_string(ERROR_DOMAIN_DESTROYED));
    printf("\tFEATURE_REMOVED = %d = \"%s\"\n", ERROR_FEATURE_REMOVED, libxl_error_to_string(ERROR_FEATURE_REMOVED));

    printf("libxl_error -- to JSON:\n");
    json_string = libxl_error_to_json(ctx, ERROR_NONSPECIFIC);
    printf("\tNONSPECIFIC = %d = %s", ERROR_NONSPECIFIC, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_VERSION);
    printf("\tVERSION = %d = %s", ERROR_VERSION, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_FAIL);
    printf("\tFAIL = %d = %s", ERROR_FAIL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_NI);
    printf("\tNI = %d = %s", ERROR_NI, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_NOMEM);
    printf("\tNOMEM = %d = %s", ERROR_NOMEM, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_INVAL);
    printf("\tINVAL = %d = %s", ERROR_INVAL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_BADFAIL);
    printf("\tBADFAIL = %d = %s", ERROR_BADFAIL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_GUEST_TIMEDOUT);
    printf("\tGUEST_TIMEDOUT = %d = %s", ERROR_GUEST_TIMEDOUT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_TIMEDOUT);
    printf("\tTIMEDOUT = %d = %s", ERROR_TIMEDOUT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_NOPARAVIRT);
    printf("\tNOPARAVIRT = %d = %s", ERROR_NOPARAVIRT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_NOT_READY);
    printf("\tNOT_READY = %d = %s", ERROR_NOT_READY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_OSEVENT_REG_FAIL);
    printf("\tOSEVENT_REG_FAIL = %d = %s", ERROR_OSEVENT_REG_FAIL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_BUFFERFULL);
    printf("\tBUFFERFULL = %d = %s", ERROR_BUFFERFULL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_UNKNOWN_CHILD);
    printf("\tUNKNOWN_CHILD = %d = %s", ERROR_UNKNOWN_CHILD, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_LOCK_FAIL);
    printf("\tLOCK_FAIL = %d = %s", ERROR_LOCK_FAIL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_JSON_CONFIG_EMPTY);
    printf("\tJSON_CONFIG_EMPTY = %d = %s", ERROR_JSON_CONFIG_EMPTY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_DEVICE_EXISTS);
    printf("\tDEVICE_EXISTS = %d = %s", ERROR_DEVICE_EXISTS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_CHECKPOINT_DEVOPS_DOES_NOT_MATCH);
    printf("\tCHECKPOINT_DEVOPS_DOES_NOT_MATCH = %d = %s", ERROR_CHECKPOINT_DEVOPS_DOES_NOT_MATCH, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_CHECKPOINT_DEVICE_NOT_SUPPORTED);
    printf("\tCHECKPOINT_DEVICE_NOT_SUPPORTED = %d = %s", ERROR_CHECKPOINT_DEVICE_NOT_SUPPORTED, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_VNUMA_CONFIG_INVALID);
    printf("\tVNUMA_CONFIG_INVALID = %d = %s", ERROR_VNUMA_CONFIG_INVALID, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_DOMAIN_NOTFOUND);
    printf("\tDOMAIN_NOTFOUND = %d = %s", ERROR_DOMAIN_NOTFOUND, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_ABORTED);
    printf("\tABORTED = %d = %s", ERROR_ABORTED, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_NOTFOUND);
    printf("\tNOTFOUND = %d = %s", ERROR_NOTFOUND, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_DOMAIN_DESTROYED);
    printf("\tDOMAIN_DESTROYED = %d = %s", ERROR_DOMAIN_DESTROYED, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_error_to_json(ctx, ERROR_FEATURE_REMOVED);
    printf("\tFEATURE_REMOVED = %d = %s", ERROR_FEATURE_REMOVED, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_error -- from string:\n");
    libxl_error_val = -1;
    rc = libxl_error_from_string("noNspecifiC", &libxl_error_val);
    printf("\tNONSPECIFIC = \"%s\" = %d (rc %d)\n", "noNspecifiC", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("verSIoN", &libxl_error_val);
    printf("\tVERSION = \"%s\" = %d (rc %d)\n", "verSIoN", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("FAiL", &libxl_error_val);
    printf("\tFAIL = \"%s\" = %d (rc %d)\n", "FAiL", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("Ni", &libxl_error_val);
    printf("\tNI = \"%s\" = %d (rc %d)\n", "Ni", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("NoMem", &libxl_error_val);
    printf("\tNOMEM = \"%s\" = %d (rc %d)\n", "NoMem", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("INVal", &libxl_error_val);
    printf("\tINVAL = \"%s\" = %d (rc %d)\n", "INVal", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("BAdfail", &libxl_error_val);
    printf("\tBADFAIL = \"%s\" = %d (rc %d)\n", "BAdfail", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("guEsT_TImeDouT", &libxl_error_val);
    printf("\tGUEST_TIMEDOUT = \"%s\" = %d (rc %d)\n", "guEsT_TImeDouT", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("timEdOut", &libxl_error_val);
    printf("\tTIMEDOUT = \"%s\" = %d (rc %d)\n", "timEdOut", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("NoPARaViRt", &libxl_error_val);
    printf("\tNOPARAVIRT = \"%s\" = %d (rc %d)\n", "NoPARaViRt", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("NOT_reaDy", &libxl_error_val);
    printf("\tNOT_READY = \"%s\" = %d (rc %d)\n", "NOT_reaDy", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("OSEvenT_Reg_fAiL", &libxl_error_val);
    printf("\tOSEVENT_REG_FAIL = \"%s\" = %d (rc %d)\n", "OSEvenT_Reg_fAiL", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("buFfERfuLl", &libxl_error_val);
    printf("\tBUFFERFULL = \"%s\" = %d (rc %d)\n", "buFfERfuLl", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("unKnown_chIld", &libxl_error_val);
    printf("\tUNKNOWN_CHILD = \"%s\" = %d (rc %d)\n", "unKnown_chIld", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("lOck_Fail", &libxl_error_val);
    printf("\tLOCK_FAIL = \"%s\" = %d (rc %d)\n", "lOck_Fail", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("JSOn_CONFig_empTy", &libxl_error_val);
    printf("\tJSON_CONFIG_EMPTY = \"%s\" = %d (rc %d)\n", "JSOn_CONFig_empTy", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("DEVICE_eXisTs", &libxl_error_val);
    printf("\tDEVICE_EXISTS = \"%s\" = %d (rc %d)\n", "DEVICE_eXisTs", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("cHeckPOint_DeVOPs_DOes_NOt_MATCH", &libxl_error_val);
    printf("\tCHECKPOINT_DEVOPS_DOES_NOT_MATCH = \"%s\" = %d (rc %d)\n", "cHeckPOint_DeVOPs_DOes_NOt_MATCH", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("CHeckPoint_dEvIce_NoT_SuppORtEd", &libxl_error_val);
    printf("\tCHECKPOINT_DEVICE_NOT_SUPPORTED = \"%s\" = %d (rc %d)\n", "CHeckPoint_dEvIce_NoT_SuppORtEd", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("VnUMA_cONfig_iNVAliD", &libxl_error_val);
    printf("\tVNUMA_CONFIG_INVALID = \"%s\" = %d (rc %d)\n", "VnUMA_cONfig_iNVAliD", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("dOMAiN_NotFoUnD", &libxl_error_val);
    printf("\tDOMAIN_NOTFOUND = \"%s\" = %d (rc %d)\n", "dOMAiN_NotFoUnD", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("aborTed", &libxl_error_val);
    printf("\tABORTED = \"%s\" = %d (rc %d)\n", "aborTed", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("notfounD", &libxl_error_val);
    printf("\tNOTFOUND = \"%s\" = %d (rc %d)\n", "notfounD", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("dOMain_destRoYed", &libxl_error_val);
    printf("\tDOMAIN_DESTROYED = \"%s\" = %d (rc %d)\n", "dOMain_destRoYed", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("FeATURE_reMOVeD", &libxl_error_val);
    printf("\tFEATURE_REMOVED = \"%s\" = %d (rc %d)\n", "FeATURE_reMOVeD", libxl_error_val, rc);
    libxl_error_val = -1;
    rc = libxl_error_from_string("AN INvALiD VALUE", &libxl_error_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN INvALiD VALUE", libxl_error_val, rc);

    printf("libxl_domain_type -- to string:\n");
    printf("\tINVALID = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_INVALID, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_INVALID));
    printf("\tHVM = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_HVM, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_HVM));
    printf("\tPV = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_PV, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_PV));
    printf("\tPVH = %d = \"%s\"\n", LIBXL_DOMAIN_TYPE_PVH, libxl_domain_type_to_string(LIBXL_DOMAIN_TYPE_PVH));

    printf("libxl_domain_type -- to JSON:\n");
    json_string = libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_INVALID);
    printf("\tINVALID = %d = %s", LIBXL_DOMAIN_TYPE_INVALID, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_HVM);
    printf("\tHVM = %d = %s", LIBXL_DOMAIN_TYPE_HVM, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_PV);
    printf("\tPV = %d = %s", LIBXL_DOMAIN_TYPE_PV, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_domain_type_to_json(ctx, LIBXL_DOMAIN_TYPE_PVH);
    printf("\tPVH = %d = %s", LIBXL_DOMAIN_TYPE_PVH, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_domain_type -- from string:\n");
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("invALID", &libxl_domain_type_val);
    printf("\tINVALID = \"%s\" = %d (rc %d)\n", "invALID", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("Hvm", &libxl_domain_type_val);
    printf("\tHVM = \"%s\" = %d (rc %d)\n", "Hvm", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("pv", &libxl_domain_type_val);
    printf("\tPV = \"%s\" = %d (rc %d)\n", "pv", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("PvH", &libxl_domain_type_val);
    printf("\tPVH = \"%s\" = %d (rc %d)\n", "PvH", libxl_domain_type_val, rc);
    libxl_domain_type_val = -1;
    rc = libxl_domain_type_from_string("aN invaLId valUE", &libxl_domain_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN invaLId valUE", libxl_domain_type_val, rc);

    printf("libxl_rdm_reserve_strategy -- to string:\n");
    printf("\tIGNORE = %d = \"%s\"\n", LIBXL_RDM_RESERVE_STRATEGY_IGNORE, libxl_rdm_reserve_strategy_to_string(LIBXL_RDM_RESERVE_STRATEGY_IGNORE));
    printf("\tHOST = %d = \"%s\"\n", LIBXL_RDM_RESERVE_STRATEGY_HOST, libxl_rdm_reserve_strategy_to_string(LIBXL_RDM_RESERVE_STRATEGY_HOST));

    printf("libxl_rdm_reserve_strategy -- to JSON:\n");
    json_string = libxl_rdm_reserve_strategy_to_json(ctx, LIBXL_RDM_RESERVE_STRATEGY_IGNORE);
    printf("\tIGNORE = %d = %s", LIBXL_RDM_RESERVE_STRATEGY_IGNORE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_rdm_reserve_strategy_to_json(ctx, LIBXL_RDM_RESERVE_STRATEGY_HOST);
    printf("\tHOST = %d = %s", LIBXL_RDM_RESERVE_STRATEGY_HOST, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_rdm_reserve_strategy -- from string:\n");
    libxl_rdm_reserve_strategy_val = -1;
    rc = libxl_rdm_reserve_strategy_from_string("IgnORE", &libxl_rdm_reserve_strategy_val);
    printf("\tIGNORE = \"%s\" = %d (rc %d)\n", "IgnORE", libxl_rdm_reserve_strategy_val, rc);
    libxl_rdm_reserve_strategy_val = -1;
    rc = libxl_rdm_reserve_strategy_from_string("hOst", &libxl_rdm_reserve_strategy_val);
    printf("\tHOST = \"%s\" = %d (rc %d)\n", "hOst", libxl_rdm_reserve_strategy_val, rc);
    libxl_rdm_reserve_strategy_val = -1;
    rc = libxl_rdm_reserve_strategy_from_string("AN iNvAliD VAlUe", &libxl_rdm_reserve_strategy_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN iNvAliD VAlUe", libxl_rdm_reserve_strategy_val, rc);

    printf("libxl_rdm_reserve_policy -- to string:\n");
    printf("\tINVALID = %d = \"%s\"\n", LIBXL_RDM_RESERVE_POLICY_INVALID, libxl_rdm_reserve_policy_to_string(LIBXL_RDM_RESERVE_POLICY_INVALID));
    printf("\tSTRICT = %d = \"%s\"\n", LIBXL_RDM_RESERVE_POLICY_STRICT, libxl_rdm_reserve_policy_to_string(LIBXL_RDM_RESERVE_POLICY_STRICT));
    printf("\tRELAXED = %d = \"%s\"\n", LIBXL_RDM_RESERVE_POLICY_RELAXED, libxl_rdm_reserve_policy_to_string(LIBXL_RDM_RESERVE_POLICY_RELAXED));

    printf("libxl_rdm_reserve_policy -- to JSON:\n");
    json_string = libxl_rdm_reserve_policy_to_json(ctx, LIBXL_RDM_RESERVE_POLICY_INVALID);
    printf("\tINVALID = %d = %s", LIBXL_RDM_RESERVE_POLICY_INVALID, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_rdm_reserve_policy_to_json(ctx, LIBXL_RDM_RESERVE_POLICY_STRICT);
    printf("\tSTRICT = %d = %s", LIBXL_RDM_RESERVE_POLICY_STRICT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_rdm_reserve_policy_to_json(ctx, LIBXL_RDM_RESERVE_POLICY_RELAXED);
    printf("\tRELAXED = %d = %s", LIBXL_RDM_RESERVE_POLICY_RELAXED, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_rdm_reserve_policy -- from string:\n");
    libxl_rdm_reserve_policy_val = -1;
    rc = libxl_rdm_reserve_policy_from_string("invaLiD", &libxl_rdm_reserve_policy_val);
    printf("\tINVALID = \"%s\" = %d (rc %d)\n", "invaLiD", libxl_rdm_reserve_policy_val, rc);
    libxl_rdm_reserve_policy_val = -1;
    rc = libxl_rdm_reserve_policy_from_string("STrICt", &libxl_rdm_reserve_policy_val);
    printf("\tSTRICT = \"%s\" = %d (rc %d)\n", "STrICt", libxl_rdm_reserve_policy_val, rc);
    libxl_rdm_reserve_policy_val = -1;
    rc = libxl_rdm_reserve_policy_from_string("rElaXED", &libxl_rdm_reserve_policy_val);
    printf("\tRELAXED = \"%s\" = %d (rc %d)\n", "rElaXED", libxl_rdm_reserve_policy_val, rc);
    libxl_rdm_reserve_policy_val = -1;
    rc = libxl_rdm_reserve_policy_from_string("aN INvALId vaLuE", &libxl_rdm_reserve_policy_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN INvALId vaLuE", libxl_rdm_reserve_policy_val, rc);

    printf("libxl_channel_connection -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_CHANNEL_CONNECTION_UNKNOWN, libxl_channel_connection_to_string(LIBXL_CHANNEL_CONNECTION_UNKNOWN));
    printf("\tPTY = %d = \"%s\"\n", LIBXL_CHANNEL_CONNECTION_PTY, libxl_channel_connection_to_string(LIBXL_CHANNEL_CONNECTION_PTY));
    printf("\tSOCKET = %d = \"%s\"\n", LIBXL_CHANNEL_CONNECTION_SOCKET, libxl_channel_connection_to_string(LIBXL_CHANNEL_CONNECTION_SOCKET));

    printf("libxl_channel_connection -- to JSON:\n");
    json_string = libxl_channel_connection_to_json(ctx, LIBXL_CHANNEL_CONNECTION_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_CHANNEL_CONNECTION_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_channel_connection_to_json(ctx, LIBXL_CHANNEL_CONNECTION_PTY);
    printf("\tPTY = %d = %s", LIBXL_CHANNEL_CONNECTION_PTY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_channel_connection_to_json(ctx, LIBXL_CHANNEL_CONNECTION_SOCKET);
    printf("\tSOCKET = %d = %s", LIBXL_CHANNEL_CONNECTION_SOCKET, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_channel_connection -- from string:\n");
    libxl_channel_connection_val = -1;
    rc = libxl_channel_connection_from_string("unKnoWN", &libxl_channel_connection_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKnoWN", libxl_channel_connection_val, rc);
    libxl_channel_connection_val = -1;
    rc = libxl_channel_connection_from_string("ptY", &libxl_channel_connection_val);
    printf("\tPTY = \"%s\" = %d (rc %d)\n", "ptY", libxl_channel_connection_val, rc);
    libxl_channel_connection_val = -1;
    rc = libxl_channel_connection_from_string("SOckeT", &libxl_channel_connection_val);
    printf("\tSOCKET = \"%s\" = %d (rc %d)\n", "SOckeT", libxl_channel_connection_val, rc);
    libxl_channel_connection_val = -1;
    rc = libxl_channel_connection_from_string("An invalId ValUE", &libxl_channel_connection_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An invalId ValUE", libxl_channel_connection_val, rc);

    printf("libxl_device_model_version -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_UNKNOWN, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_UNKNOWN));
    printf("\tQEMU_XEN_TRADITIONAL = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL));
    printf("\tQEMU_XEN = %d = \"%s\"\n", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN, libxl_device_model_version_to_string(LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN));

    printf("libxl_device_model_version -- to JSON:\n");
    json_string = libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_DEVICE_MODEL_VERSION_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL);
    printf("\tQEMU_XEN_TRADITIONAL = %d = %s", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_device_model_version_to_json(ctx, LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN);
    printf("\tQEMU_XEN = %d = %s", LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_device_model_version -- from string:\n");
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("UNknowN", &libxl_device_model_version_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UNknowN", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("Qemu_XeN_tRaDITIOnAL", &libxl_device_model_version_val);
    printf("\tQEMU_XEN_TRADITIONAL = \"%s\" = %d (rc %d)\n", "Qemu_XeN_tRaDITIOnAL", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("QeMU_xen", &libxl_device_model_version_val);
    printf("\tQEMU_XEN = \"%s\" = %d (rc %d)\n", "QeMU_xen", libxl_device_model_version_val, rc);
    libxl_device_model_version_val = -1;
    rc = libxl_device_model_version_from_string("An iNvalID ValUe", &libxl_device_model_version_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An iNvalID ValUe", libxl_device_model_version_val, rc);

    printf("libxl_console_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_UNKNOWN, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_UNKNOWN));
    printf("\tSERIAL = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_SERIAL, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_SERIAL));
    printf("\tPV = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_PV, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_PV));
    printf("\tVUART = %d = \"%s\"\n", LIBXL_CONSOLE_TYPE_VUART, libxl_console_type_to_string(LIBXL_CONSOLE_TYPE_VUART));

    printf("libxl_console_type -- to JSON:\n");
    json_string = libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_CONSOLE_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_SERIAL);
    printf("\tSERIAL = %d = %s", LIBXL_CONSOLE_TYPE_SERIAL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_PV);
    printf("\tPV = %d = %s", LIBXL_CONSOLE_TYPE_PV, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_console_type_to_json(ctx, LIBXL_CONSOLE_TYPE_VUART);
    printf("\tVUART = %d = %s", LIBXL_CONSOLE_TYPE_VUART, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_console_type -- from string:\n");
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("unKnOwN", &libxl_console_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKnOwN", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("sERiAL", &libxl_console_type_val);
    printf("\tSERIAL = \"%s\" = %d (rc %d)\n", "sERiAL", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("PV", &libxl_console_type_val);
    printf("\tPV = \"%s\" = %d (rc %d)\n", "PV", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("VUArt", &libxl_console_type_val);
    printf("\tVUART = \"%s\" = %d (rc %d)\n", "VUArt", libxl_console_type_val, rc);
    libxl_console_type_val = -1;
    rc = libxl_console_type_from_string("AN iNVaLiD VaLUE", &libxl_console_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN iNVaLiD VaLUE", libxl_console_type_val, rc);

    printf("libxl_disk_format -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DISK_FORMAT_UNKNOWN, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_UNKNOWN));
    printf("\tQCOW = %d = \"%s\"\n", LIBXL_DISK_FORMAT_QCOW, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_QCOW));
    printf("\tQCOW2 = %d = \"%s\"\n", LIBXL_DISK_FORMAT_QCOW2, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_QCOW2));
    printf("\tVHD = %d = \"%s\"\n", LIBXL_DISK_FORMAT_VHD, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_VHD));
    printf("\tRAW = %d = \"%s\"\n", LIBXL_DISK_FORMAT_RAW, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_RAW));
    printf("\tEMPTY = %d = \"%s\"\n", LIBXL_DISK_FORMAT_EMPTY, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_EMPTY));
    printf("\tQED = %d = \"%s\"\n", LIBXL_DISK_FORMAT_QED, libxl_disk_format_to_string(LIBXL_DISK_FORMAT_QED));

    printf("libxl_disk_format -- to JSON:\n");
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_DISK_FORMAT_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_QCOW);
    printf("\tQCOW = %d = %s", LIBXL_DISK_FORMAT_QCOW, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_QCOW2);
    printf("\tQCOW2 = %d = %s", LIBXL_DISK_FORMAT_QCOW2, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_VHD);
    printf("\tVHD = %d = %s", LIBXL_DISK_FORMAT_VHD, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_RAW);
    printf("\tRAW = %d = %s", LIBXL_DISK_FORMAT_RAW, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_EMPTY);
    printf("\tEMPTY = %d = %s", LIBXL_DISK_FORMAT_EMPTY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_format_to_json(ctx, LIBXL_DISK_FORMAT_QED);
    printf("\tQED = %d = %s", LIBXL_DISK_FORMAT_QED, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_disk_format -- from string:\n");
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("UnknOWn", &libxl_disk_format_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UnknOWn", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("QcOW", &libxl_disk_format_val);
    printf("\tQCOW = \"%s\" = %d (rc %d)\n", "QcOW", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("QcOW2", &libxl_disk_format_val);
    printf("\tQCOW2 = \"%s\" = %d (rc %d)\n", "QcOW2", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("Vhd", &libxl_disk_format_val);
    printf("\tVHD = \"%s\" = %d (rc %d)\n", "Vhd", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("raW", &libxl_disk_format_val);
    printf("\tRAW = \"%s\" = %d (rc %d)\n", "raW", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("emptY", &libxl_disk_format_val);
    printf("\tEMPTY = \"%s\" = %d (rc %d)\n", "emptY", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("qED", &libxl_disk_format_val);
    printf("\tQED = \"%s\" = %d (rc %d)\n", "qED", libxl_disk_format_val, rc);
    libxl_disk_format_val = -1;
    rc = libxl_disk_format_from_string("aN iNvalid Value", &libxl_disk_format_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN iNvalid Value", libxl_disk_format_val, rc);

    printf("libxl_disk_backend -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_DISK_BACKEND_UNKNOWN, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_UNKNOWN));
    printf("\tPHY = %d = \"%s\"\n", LIBXL_DISK_BACKEND_PHY, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_PHY));
    printf("\tTAP = %d = \"%s\"\n", LIBXL_DISK_BACKEND_TAP, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_TAP));
    printf("\tQDISK = %d = \"%s\"\n", LIBXL_DISK_BACKEND_QDISK, libxl_disk_backend_to_string(LIBXL_DISK_BACKEND_QDISK));

    printf("libxl_disk_backend -- to JSON:\n");
    json_string = libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_DISK_BACKEND_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_PHY);
    printf("\tPHY = %d = %s", LIBXL_DISK_BACKEND_PHY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_TAP);
    printf("\tTAP = %d = %s", LIBXL_DISK_BACKEND_TAP, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_disk_backend_to_json(ctx, LIBXL_DISK_BACKEND_QDISK);
    printf("\tQDISK = %d = %s", LIBXL_DISK_BACKEND_QDISK, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_disk_backend -- from string:\n");
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("Unknown", &libxl_disk_backend_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "Unknown", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("PhY", &libxl_disk_backend_val);
    printf("\tPHY = \"%s\" = %d (rc %d)\n", "PhY", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("Tap", &libxl_disk_backend_val);
    printf("\tTAP = \"%s\" = %d (rc %d)\n", "Tap", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("qDISK", &libxl_disk_backend_val);
    printf("\tQDISK = \"%s\" = %d (rc %d)\n", "qDISK", libxl_disk_backend_val, rc);
    libxl_disk_backend_val = -1;
    rc = libxl_disk_backend_from_string("AN InvALId vAlue", &libxl_disk_backend_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN InvALId vAlue", libxl_disk_backend_val, rc);

    printf("libxl_nic_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_NIC_TYPE_UNKNOWN, libxl_nic_type_to_string(LIBXL_NIC_TYPE_UNKNOWN));
    printf("\tVIF_IOEMU = %d = \"%s\"\n", LIBXL_NIC_TYPE_VIF_IOEMU, libxl_nic_type_to_string(LIBXL_NIC_TYPE_VIF_IOEMU));
    printf("\tVIF = %d = \"%s\"\n", LIBXL_NIC_TYPE_VIF, libxl_nic_type_to_string(LIBXL_NIC_TYPE_VIF));

    printf("libxl_nic_type -- to JSON:\n");
    json_string = libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_NIC_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_VIF_IOEMU);
    printf("\tVIF_IOEMU = %d = %s", LIBXL_NIC_TYPE_VIF_IOEMU, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_nic_type_to_json(ctx, LIBXL_NIC_TYPE_VIF);
    printf("\tVIF = %d = %s", LIBXL_NIC_TYPE_VIF, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_nic_type -- from string:\n");
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("unkNOWn", &libxl_nic_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unkNOWn", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("ViF_IoEMu", &libxl_nic_type_val);
    printf("\tVIF_IOEMU = \"%s\" = %d (rc %d)\n", "ViF_IoEMu", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("vif", &libxl_nic_type_val);
    printf("\tVIF = \"%s\" = %d (rc %d)\n", "vif", libxl_nic_type_val, rc);
    libxl_nic_type_val = -1;
    rc = libxl_nic_type_from_string("AN invALID VaLUE", &libxl_nic_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN invALID VaLUE", libxl_nic_type_val, rc);

    printf("libxl_action_on_shutdown -- to string:\n");
    printf("\tDESTROY = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_DESTROY, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_DESTROY));
    printf("\tRESTART = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_RESTART, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_RESTART));
    printf("\tRESTART_RENAME = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME));
    printf("\tPRESERVE = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_PRESERVE, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_PRESERVE));
    printf("\tCOREDUMP_DESTROY = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY));
    printf("\tCOREDUMP_RESTART = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART));
    printf("\tSOFT_RESET = %d = \"%s\"\n", LIBXL_ACTION_ON_SHUTDOWN_SOFT_RESET, libxl_action_on_shutdown_to_string(LIBXL_ACTION_ON_SHUTDOWN_SOFT_RESET));

    printf("libxl_action_on_shutdown -- to JSON:\n");
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_DESTROY);
    printf("\tDESTROY = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_DESTROY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_RESTART);
    printf("\tRESTART = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_RESTART, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME);
    printf("\tRESTART_RENAME = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_PRESERVE);
    printf("\tPRESERVE = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_PRESERVE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY);
    printf("\tCOREDUMP_DESTROY = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART);
    printf("\tCOREDUMP_RESTART = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_action_on_shutdown_to_json(ctx, LIBXL_ACTION_ON_SHUTDOWN_SOFT_RESET);
    printf("\tSOFT_RESET = %d = %s", LIBXL_ACTION_ON_SHUTDOWN_SOFT_RESET, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_action_on_shutdown -- from string:\n");
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("dESTRoY", &libxl_action_on_shutdown_val);
    printf("\tDESTROY = \"%s\" = %d (rc %d)\n", "dESTRoY", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("REsTArT", &libxl_action_on_shutdown_val);
    printf("\tRESTART = \"%s\" = %d (rc %d)\n", "REsTArT", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("RestART_rEname", &libxl_action_on_shutdown_val);
    printf("\tRESTART_RENAME = \"%s\" = %d (rc %d)\n", "RestART_rEname", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("PReserVe", &libxl_action_on_shutdown_val);
    printf("\tPRESERVE = \"%s\" = %d (rc %d)\n", "PReserVe", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("CorEduMp_DesTroY", &libxl_action_on_shutdown_val);
    printf("\tCOREDUMP_DESTROY = \"%s\" = %d (rc %d)\n", "CorEduMp_DesTroY", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("cOrEDumP_ResTArt", &libxl_action_on_shutdown_val);
    printf("\tCOREDUMP_RESTART = \"%s\" = %d (rc %d)\n", "cOrEDumP_ResTArt", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("soft_RESET", &libxl_action_on_shutdown_val);
    printf("\tSOFT_RESET = \"%s\" = %d (rc %d)\n", "soft_RESET", libxl_action_on_shutdown_val, rc);
    libxl_action_on_shutdown_val = -1;
    rc = libxl_action_on_shutdown_from_string("an INvalID VALue", &libxl_action_on_shutdown_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an INvalID VALue", libxl_action_on_shutdown_val, rc);

    printf("libxl_trigger -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_TRIGGER_UNKNOWN, libxl_trigger_to_string(LIBXL_TRIGGER_UNKNOWN));
    printf("\tPOWER = %d = \"%s\"\n", LIBXL_TRIGGER_POWER, libxl_trigger_to_string(LIBXL_TRIGGER_POWER));
    printf("\tSLEEP = %d = \"%s\"\n", LIBXL_TRIGGER_SLEEP, libxl_trigger_to_string(LIBXL_TRIGGER_SLEEP));
    printf("\tNMI = %d = \"%s\"\n", LIBXL_TRIGGER_NMI, libxl_trigger_to_string(LIBXL_TRIGGER_NMI));
    printf("\tINIT = %d = \"%s\"\n", LIBXL_TRIGGER_INIT, libxl_trigger_to_string(LIBXL_TRIGGER_INIT));
    printf("\tRESET = %d = \"%s\"\n", LIBXL_TRIGGER_RESET, libxl_trigger_to_string(LIBXL_TRIGGER_RESET));
    printf("\tS3RESUME = %d = \"%s\"\n", LIBXL_TRIGGER_S3RESUME, libxl_trigger_to_string(LIBXL_TRIGGER_S3RESUME));

    printf("libxl_trigger -- to JSON:\n");
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_TRIGGER_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_POWER);
    printf("\tPOWER = %d = %s", LIBXL_TRIGGER_POWER, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_SLEEP);
    printf("\tSLEEP = %d = %s", LIBXL_TRIGGER_SLEEP, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_NMI);
    printf("\tNMI = %d = %s", LIBXL_TRIGGER_NMI, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_INIT);
    printf("\tINIT = %d = %s", LIBXL_TRIGGER_INIT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_RESET);
    printf("\tRESET = %d = %s", LIBXL_TRIGGER_RESET, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_trigger_to_json(ctx, LIBXL_TRIGGER_S3RESUME);
    printf("\tS3RESUME = %d = %s", LIBXL_TRIGGER_S3RESUME, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_trigger -- from string:\n");
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("uNKnoWn", &libxl_trigger_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNKnoWn", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("POWer", &libxl_trigger_val);
    printf("\tPOWER = \"%s\" = %d (rc %d)\n", "POWer", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("SLEeP", &libxl_trigger_val);
    printf("\tSLEEP = \"%s\" = %d (rc %d)\n", "SLEeP", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("NMI", &libxl_trigger_val);
    printf("\tNMI = \"%s\" = %d (rc %d)\n", "NMI", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("inIT", &libxl_trigger_val);
    printf("\tINIT = \"%s\" = %d (rc %d)\n", "inIT", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("rEsET", &libxl_trigger_val);
    printf("\tRESET = \"%s\" = %d (rc %d)\n", "rEsET", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("s3rEsUmE", &libxl_trigger_val);
    printf("\tS3RESUME = \"%s\" = %d (rc %d)\n", "s3rEsUmE", libxl_trigger_val, rc);
    libxl_trigger_val = -1;
    rc = libxl_trigger_from_string("An invALid vAluE", &libxl_trigger_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An invALid vAluE", libxl_trigger_val, rc);

    printf("libxl_tsc_mode -- to string:\n");
    printf("\tDEFAULT = %d = \"%s\"\n", LIBXL_TSC_MODE_DEFAULT, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_DEFAULT));
    printf("\tALWAYS_EMULATE = %d = \"%s\"\n", LIBXL_TSC_MODE_ALWAYS_EMULATE, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_ALWAYS_EMULATE));
    printf("\tNATIVE = %d = \"%s\"\n", LIBXL_TSC_MODE_NATIVE, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_NATIVE));
    printf("\tNATIVE_PARAVIRT = %d = \"%s\"\n", LIBXL_TSC_MODE_NATIVE_PARAVIRT, libxl_tsc_mode_to_string(LIBXL_TSC_MODE_NATIVE_PARAVIRT));

    printf("libxl_tsc_mode -- to JSON:\n");
    json_string = libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_DEFAULT);
    printf("\tDEFAULT = %d = %s", LIBXL_TSC_MODE_DEFAULT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_ALWAYS_EMULATE);
    printf("\tALWAYS_EMULATE = %d = %s", LIBXL_TSC_MODE_ALWAYS_EMULATE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_NATIVE);
    printf("\tNATIVE = %d = %s", LIBXL_TSC_MODE_NATIVE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_tsc_mode_to_json(ctx, LIBXL_TSC_MODE_NATIVE_PARAVIRT);
    printf("\tNATIVE_PARAVIRT = %d = %s", LIBXL_TSC_MODE_NATIVE_PARAVIRT, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_tsc_mode -- from string:\n");
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("dEfAulT", &libxl_tsc_mode_val);
    printf("\tDEFAULT = \"%s\" = %d (rc %d)\n", "dEfAulT", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("AlwayS_EMulAte", &libxl_tsc_mode_val);
    printf("\tALWAYS_EMULATE = \"%s\" = %d (rc %d)\n", "AlwayS_EMulAte", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("NAtIve", &libxl_tsc_mode_val);
    printf("\tNATIVE = \"%s\" = %d (rc %d)\n", "NAtIve", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("NATiVe_pArAViRT", &libxl_tsc_mode_val);
    printf("\tNATIVE_PARAVIRT = \"%s\" = %d (rc %d)\n", "NATiVe_pArAViRT", libxl_tsc_mode_val, rc);
    libxl_tsc_mode_val = -1;
    rc = libxl_tsc_mode_from_string("AN inVaLID VAluE", &libxl_tsc_mode_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN inVaLID VAluE", libxl_tsc_mode_val, rc);

    printf("libxl_gfx_passthru_kind -- to string:\n");
    printf("\tDEFAULT = %d = \"%s\"\n", LIBXL_GFX_PASSTHRU_KIND_DEFAULT, libxl_gfx_passthru_kind_to_string(LIBXL_GFX_PASSTHRU_KIND_DEFAULT));
    printf("\tIGD = %d = \"%s\"\n", LIBXL_GFX_PASSTHRU_KIND_IGD, libxl_gfx_passthru_kind_to_string(LIBXL_GFX_PASSTHRU_KIND_IGD));

    printf("libxl_gfx_passthru_kind -- to JSON:\n");
    json_string = libxl_gfx_passthru_kind_to_json(ctx, LIBXL_GFX_PASSTHRU_KIND_DEFAULT);
    printf("\tDEFAULT = %d = %s", LIBXL_GFX_PASSTHRU_KIND_DEFAULT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_gfx_passthru_kind_to_json(ctx, LIBXL_GFX_PASSTHRU_KIND_IGD);
    printf("\tIGD = %d = %s", LIBXL_GFX_PASSTHRU_KIND_IGD, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_gfx_passthru_kind -- from string:\n");
    libxl_gfx_passthru_kind_val = -1;
    rc = libxl_gfx_passthru_kind_from_string("dEFauLT", &libxl_gfx_passthru_kind_val);
    printf("\tDEFAULT = \"%s\" = %d (rc %d)\n", "dEFauLT", libxl_gfx_passthru_kind_val, rc);
    libxl_gfx_passthru_kind_val = -1;
    rc = libxl_gfx_passthru_kind_from_string("igd", &libxl_gfx_passthru_kind_val);
    printf("\tIGD = \"%s\" = %d (rc %d)\n", "igd", libxl_gfx_passthru_kind_val, rc);
    libxl_gfx_passthru_kind_val = -1;
    rc = libxl_gfx_passthru_kind_from_string("AN invALiD vaLUE", &libxl_gfx_passthru_kind_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN invALiD vaLUE", libxl_gfx_passthru_kind_val, rc);

    printf("libxl_timer_mode -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_TIMER_MODE_UNKNOWN, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_UNKNOWN));
    printf("\tDELAY_FOR_MISSED_TICKS = %d = \"%s\"\n", LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_DELAY_FOR_MISSED_TICKS = %d = \"%s\"\n", LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS));
    printf("\tNO_MISSED_TICKS_PENDING = %d = \"%s\"\n", LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING));
    printf("\tONE_MISSED_TICK_PENDING = %d = \"%s\"\n", LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING, libxl_timer_mode_to_string(LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING));

    printf("libxl_timer_mode -- to JSON:\n");
    json_string = libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_TIMER_MODE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS);
    printf("\tDELAY_FOR_MISSED_TICKS = %d = %s", LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS);
    printf("\tNO_DELAY_FOR_MISSED_TICKS = %d = %s", LIBXL_TIMER_MODE_NO_DELAY_FOR_MISSED_TICKS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING);
    printf("\tNO_MISSED_TICKS_PENDING = %d = %s", LIBXL_TIMER_MODE_NO_MISSED_TICKS_PENDING, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_timer_mode_to_json(ctx, LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING);
    printf("\tONE_MISSED_TICK_PENDING = %d = %s", LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_timer_mode -- from string:\n");
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("uNkNOwN", &libxl_timer_mode_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNkNOwN", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("delay_For_MIsSeD_tiCkS", &libxl_timer_mode_val);
    printf("\tDELAY_FOR_MISSED_TICKS = \"%s\" = %d (rc %d)\n", "delay_For_MIsSeD_tiCkS", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("No_DeLAY_fOR_miSseD_TicKs", &libxl_timer_mode_val);
    printf("\tNO_DELAY_FOR_MISSED_TICKS = \"%s\" = %d (rc %d)\n", "No_DeLAY_fOR_miSseD_TicKs", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("nO_MIssEd_TIcKs_PeNDiNG", &libxl_timer_mode_val);
    printf("\tNO_MISSED_TICKS_PENDING = \"%s\" = %d (rc %d)\n", "nO_MIssEd_TIcKs_PeNDiNG", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("onE_MISSeD_ticK_pendiNg", &libxl_timer_mode_val);
    printf("\tONE_MISSED_TICK_PENDING = \"%s\" = %d (rc %d)\n", "onE_MISSeD_ticK_pendiNg", libxl_timer_mode_val, rc);
    libxl_timer_mode_val = -1;
    rc = libxl_timer_mode_from_string("aN iNVaLiD VAlUE", &libxl_timer_mode_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN iNVaLiD VAlUE", libxl_timer_mode_val, rc);

    printf("libxl_bios_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_BIOS_TYPE_UNKNOWN, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_UNKNOWN));
    printf("\tROMBIOS = %d = \"%s\"\n", LIBXL_BIOS_TYPE_ROMBIOS, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_ROMBIOS));
    printf("\tSEABIOS = %d = \"%s\"\n", LIBXL_BIOS_TYPE_SEABIOS, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_SEABIOS));
    printf("\tOVMF = %d = \"%s\"\n", LIBXL_BIOS_TYPE_OVMF, libxl_bios_type_to_string(LIBXL_BIOS_TYPE_OVMF));

    printf("libxl_bios_type -- to JSON:\n");
    json_string = libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_BIOS_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_ROMBIOS);
    printf("\tROMBIOS = %d = %s", LIBXL_BIOS_TYPE_ROMBIOS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_SEABIOS);
    printf("\tSEABIOS = %d = %s", LIBXL_BIOS_TYPE_SEABIOS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_bios_type_to_json(ctx, LIBXL_BIOS_TYPE_OVMF);
    printf("\tOVMF = %d = %s", LIBXL_BIOS_TYPE_OVMF, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_bios_type -- from string:\n");
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("uNknowN", &libxl_bios_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNknowN", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("ROMbIOs", &libxl_bios_type_val);
    printf("\tROMBIOS = \"%s\" = %d (rc %d)\n", "ROMbIOs", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("sEAbIOS", &libxl_bios_type_val);
    printf("\tSEABIOS = \"%s\" = %d (rc %d)\n", "sEAbIOS", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("OVMf", &libxl_bios_type_val);
    printf("\tOVMF = \"%s\" = %d (rc %d)\n", "OVMf", libxl_bios_type_val, rc);
    libxl_bios_type_val = -1;
    rc = libxl_bios_type_from_string("AN INValiD vaLUe", &libxl_bios_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN INValiD vaLUe", libxl_bios_type_val, rc);

    printf("libxl_scheduler -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_SCHEDULER_UNKNOWN, libxl_scheduler_to_string(LIBXL_SCHEDULER_UNKNOWN));
    printf("\tSEDF = %d = \"%s\"\n", LIBXL_SCHEDULER_SEDF, libxl_scheduler_to_string(LIBXL_SCHEDULER_SEDF));
    printf("\tCREDIT = %d = \"%s\"\n", LIBXL_SCHEDULER_CREDIT, libxl_scheduler_to_string(LIBXL_SCHEDULER_CREDIT));
    printf("\tCREDIT2 = %d = \"%s\"\n", LIBXL_SCHEDULER_CREDIT2, libxl_scheduler_to_string(LIBXL_SCHEDULER_CREDIT2));
    printf("\tARINC653 = %d = \"%s\"\n", LIBXL_SCHEDULER_ARINC653, libxl_scheduler_to_string(LIBXL_SCHEDULER_ARINC653));
    printf("\tRTDS = %d = \"%s\"\n", LIBXL_SCHEDULER_RTDS, libxl_scheduler_to_string(LIBXL_SCHEDULER_RTDS));
    printf("\tNULL = %d = \"%s\"\n", LIBXL_SCHEDULER_NULL, libxl_scheduler_to_string(LIBXL_SCHEDULER_NULL));

    printf("libxl_scheduler -- to JSON:\n");
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_SCHEDULER_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_SEDF);
    printf("\tSEDF = %d = %s", LIBXL_SCHEDULER_SEDF, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_CREDIT);
    printf("\tCREDIT = %d = %s", LIBXL_SCHEDULER_CREDIT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_CREDIT2);
    printf("\tCREDIT2 = %d = %s", LIBXL_SCHEDULER_CREDIT2, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_ARINC653);
    printf("\tARINC653 = %d = %s", LIBXL_SCHEDULER_ARINC653, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_RTDS);
    printf("\tRTDS = %d = %s", LIBXL_SCHEDULER_RTDS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_scheduler_to_json(ctx, LIBXL_SCHEDULER_NULL);
    printf("\tNULL = %d = %s", LIBXL_SCHEDULER_NULL, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_scheduler -- from string:\n");
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("uNKNOwn", &libxl_scheduler_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNKNOwn", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("SedF", &libxl_scheduler_val);
    printf("\tSEDF = \"%s\" = %d (rc %d)\n", "SedF", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("CREdiT", &libxl_scheduler_val);
    printf("\tCREDIT = \"%s\" = %d (rc %d)\n", "CREdiT", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("cREDIt2", &libxl_scheduler_val);
    printf("\tCREDIT2 = \"%s\" = %d (rc %d)\n", "cREDIt2", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("ArinC653", &libxl_scheduler_val);
    printf("\tARINC653 = \"%s\" = %d (rc %d)\n", "ArinC653", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("RTDs", &libxl_scheduler_val);
    printf("\tRTDS = \"%s\" = %d (rc %d)\n", "RTDs", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("nuLL", &libxl_scheduler_val);
    printf("\tNULL = \"%s\" = %d (rc %d)\n", "nuLL", libxl_scheduler_val, rc);
    libxl_scheduler_val = -1;
    rc = libxl_scheduler_from_string("an iNvAlid VALUe", &libxl_scheduler_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an iNvAlid VALUe", libxl_scheduler_val, rc);

    printf("libxl_shutdown_reason -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_UNKNOWN, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_UNKNOWN));
    printf("\tPOWEROFF = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_POWEROFF, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_POWEROFF));
    printf("\tREBOOT = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_REBOOT, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_REBOOT));
    printf("\tSUSPEND = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_SUSPEND, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_SUSPEND));
    printf("\tCRASH = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_CRASH, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_CRASH));
    printf("\tWATCHDOG = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_WATCHDOG, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_WATCHDOG));
    printf("\tSOFT_RESET = %d = \"%s\"\n", LIBXL_SHUTDOWN_REASON_SOFT_RESET, libxl_shutdown_reason_to_string(LIBXL_SHUTDOWN_REASON_SOFT_RESET));

    printf("libxl_shutdown_reason -- to JSON:\n");
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_SHUTDOWN_REASON_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_POWEROFF);
    printf("\tPOWEROFF = %d = %s", LIBXL_SHUTDOWN_REASON_POWEROFF, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_REBOOT);
    printf("\tREBOOT = %d = %s", LIBXL_SHUTDOWN_REASON_REBOOT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_SUSPEND);
    printf("\tSUSPEND = %d = %s", LIBXL_SHUTDOWN_REASON_SUSPEND, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_CRASH);
    printf("\tCRASH = %d = %s", LIBXL_SHUTDOWN_REASON_CRASH, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_WATCHDOG);
    printf("\tWATCHDOG = %d = %s", LIBXL_SHUTDOWN_REASON_WATCHDOG, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_shutdown_reason_to_json(ctx, LIBXL_SHUTDOWN_REASON_SOFT_RESET);
    printf("\tSOFT_RESET = %d = %s", LIBXL_SHUTDOWN_REASON_SOFT_RESET, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_shutdown_reason -- from string:\n");
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("UnKNOwn", &libxl_shutdown_reason_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UnKNOwn", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("pOweRoFF", &libxl_shutdown_reason_val);
    printf("\tPOWEROFF = \"%s\" = %d (rc %d)\n", "pOweRoFF", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("reBOOt", &libxl_shutdown_reason_val);
    printf("\tREBOOT = \"%s\" = %d (rc %d)\n", "reBOOt", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("suSpend", &libxl_shutdown_reason_val);
    printf("\tSUSPEND = \"%s\" = %d (rc %d)\n", "suSpend", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("crASH", &libxl_shutdown_reason_val);
    printf("\tCRASH = \"%s\" = %d (rc %d)\n", "crASH", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("WATchDOg", &libxl_shutdown_reason_val);
    printf("\tWATCHDOG = \"%s\" = %d (rc %d)\n", "WATchDOg", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("soFT_ResET", &libxl_shutdown_reason_val);
    printf("\tSOFT_RESET = \"%s\" = %d (rc %d)\n", "soFT_ResET", libxl_shutdown_reason_val, rc);
    libxl_shutdown_reason_val = -1;
    rc = libxl_shutdown_reason_from_string("aN inVAliD VALUe", &libxl_shutdown_reason_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN inVAliD VALUe", libxl_shutdown_reason_val, rc);

    printf("libxl_vga_interface_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_UNKNOWN, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_UNKNOWN));
    printf("\tCIRRUS = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_CIRRUS, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_CIRRUS));
    printf("\tSTD = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_STD, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_STD));
    printf("\tNONE = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_NONE, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_NONE));
    printf("\tQXL = %d = \"%s\"\n", LIBXL_VGA_INTERFACE_TYPE_QXL, libxl_vga_interface_type_to_string(LIBXL_VGA_INTERFACE_TYPE_QXL));

    printf("libxl_vga_interface_type -- to JSON:\n");
    json_string = libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_VGA_INTERFACE_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_CIRRUS);
    printf("\tCIRRUS = %d = %s", LIBXL_VGA_INTERFACE_TYPE_CIRRUS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_STD);
    printf("\tSTD = %d = %s", LIBXL_VGA_INTERFACE_TYPE_STD, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_NONE);
    printf("\tNONE = %d = %s", LIBXL_VGA_INTERFACE_TYPE_NONE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vga_interface_type_to_json(ctx, LIBXL_VGA_INTERFACE_TYPE_QXL);
    printf("\tQXL = %d = %s", LIBXL_VGA_INTERFACE_TYPE_QXL, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_vga_interface_type -- from string:\n");
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("uNknown", &libxl_vga_interface_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "uNknown", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("CiRRUs", &libxl_vga_interface_type_val);
    printf("\tCIRRUS = \"%s\" = %d (rc %d)\n", "CiRRUs", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("Std", &libxl_vga_interface_type_val);
    printf("\tSTD = \"%s\" = %d (rc %d)\n", "Std", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("nONE", &libxl_vga_interface_type_val);
    printf("\tNONE = \"%s\" = %d (rc %d)\n", "nONE", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("qxl", &libxl_vga_interface_type_val);
    printf("\tQXL = \"%s\" = %d (rc %d)\n", "qxl", libxl_vga_interface_type_val, rc);
    libxl_vga_interface_type_val = -1;
    rc = libxl_vga_interface_type_from_string("aN InvALiD ValUE", &libxl_vga_interface_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN InvALiD ValUE", libxl_vga_interface_type_val, rc);

    printf("libxl_vendor_device -- to string:\n");
    printf("\tNONE = %d = \"%s\"\n", LIBXL_VENDOR_DEVICE_NONE, libxl_vendor_device_to_string(LIBXL_VENDOR_DEVICE_NONE));
    printf("\tXENSERVER = %d = \"%s\"\n", LIBXL_VENDOR_DEVICE_XENSERVER, libxl_vendor_device_to_string(LIBXL_VENDOR_DEVICE_XENSERVER));

    printf("libxl_vendor_device -- to JSON:\n");
    json_string = libxl_vendor_device_to_json(ctx, LIBXL_VENDOR_DEVICE_NONE);
    printf("\tNONE = %d = %s", LIBXL_VENDOR_DEVICE_NONE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vendor_device_to_json(ctx, LIBXL_VENDOR_DEVICE_XENSERVER);
    printf("\tXENSERVER = %d = %s", LIBXL_VENDOR_DEVICE_XENSERVER, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_vendor_device -- from string:\n");
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("nONe", &libxl_vendor_device_val);
    printf("\tNONE = \"%s\" = %d (rc %d)\n", "nONe", libxl_vendor_device_val, rc);
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("xenseRVER", &libxl_vendor_device_val);
    printf("\tXENSERVER = \"%s\" = %d (rc %d)\n", "xenseRVER", libxl_vendor_device_val, rc);
    libxl_vendor_device_val = -1;
    rc = libxl_vendor_device_from_string("An inVALid VAlUe", &libxl_vendor_device_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An inVALid VAlUe", libxl_vendor_device_val, rc);

    printf("libxl_viridian_enlightenment -- to string:\n");
    printf("\tBASE = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_BASE, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_BASE));
    printf("\tFREQ = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_FREQ, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_FREQ));
    printf("\tTIME_REF_COUNT = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_TIME_REF_COUNT, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_TIME_REF_COUNT));
    printf("\tREFERENCE_TSC = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_REFERENCE_TSC, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_REFERENCE_TSC));
    printf("\tHCALL_REMOTE_TLB_FLUSH = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_HCALL_REMOTE_TLB_FLUSH, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_HCALL_REMOTE_TLB_FLUSH));
    printf("\tAPIC_ASSIST = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_APIC_ASSIST, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_APIC_ASSIST));
    printf("\tCRASH_CTL = %d = \"%s\"\n", LIBXL_VIRIDIAN_ENLIGHTENMENT_CRASH_CTL, libxl_viridian_enlightenment_to_string(LIBXL_VIRIDIAN_ENLIGHTENMENT_CRASH_CTL));

    printf("libxl_viridian_enlightenment -- to JSON:\n");
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_BASE);
    printf("\tBASE = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_BASE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_FREQ);
    printf("\tFREQ = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_FREQ, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_TIME_REF_COUNT);
    printf("\tTIME_REF_COUNT = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_TIME_REF_COUNT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_REFERENCE_TSC);
    printf("\tREFERENCE_TSC = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_REFERENCE_TSC, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_HCALL_REMOTE_TLB_FLUSH);
    printf("\tHCALL_REMOTE_TLB_FLUSH = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_HCALL_REMOTE_TLB_FLUSH, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_APIC_ASSIST);
    printf("\tAPIC_ASSIST = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_APIC_ASSIST, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_viridian_enlightenment_to_json(ctx, LIBXL_VIRIDIAN_ENLIGHTENMENT_CRASH_CTL);
    printf("\tCRASH_CTL = %d = %s", LIBXL_VIRIDIAN_ENLIGHTENMENT_CRASH_CTL, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_viridian_enlightenment -- from string:\n");
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("bASE", &libxl_viridian_enlightenment_val);
    printf("\tBASE = \"%s\" = %d (rc %d)\n", "bASE", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("FREQ", &libxl_viridian_enlightenment_val);
    printf("\tFREQ = \"%s\" = %d (rc %d)\n", "FREQ", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("TIMe_REf_CoUnT", &libxl_viridian_enlightenment_val);
    printf("\tTIME_REF_COUNT = \"%s\" = %d (rc %d)\n", "TIMe_REf_CoUnT", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("REfEreNce_tSc", &libxl_viridian_enlightenment_val);
    printf("\tREFERENCE_TSC = \"%s\" = %d (rc %d)\n", "REfEreNce_tSc", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("HcALl_rEmoTE_tlB_fLUsh", &libxl_viridian_enlightenment_val);
    printf("\tHCALL_REMOTE_TLB_FLUSH = \"%s\" = %d (rc %d)\n", "HcALl_rEmoTE_tlB_fLUsh", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("apIC_ASsIst", &libxl_viridian_enlightenment_val);
    printf("\tAPIC_ASSIST = \"%s\" = %d (rc %d)\n", "apIC_ASsIst", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("CraSH_ctL", &libxl_viridian_enlightenment_val);
    printf("\tCRASH_CTL = \"%s\" = %d (rc %d)\n", "CraSH_ctL", libxl_viridian_enlightenment_val, rc);
    libxl_viridian_enlightenment_val = -1;
    rc = libxl_viridian_enlightenment_from_string("an INValId vaLUe", &libxl_viridian_enlightenment_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an INValId vaLUe", libxl_viridian_enlightenment_val, rc);

    printf("libxl_hdtype -- to string:\n");
    printf("\tIDE = %d = \"%s\"\n", LIBXL_HDTYPE_IDE, libxl_hdtype_to_string(LIBXL_HDTYPE_IDE));
    printf("\tAHCI = %d = \"%s\"\n", LIBXL_HDTYPE_AHCI, libxl_hdtype_to_string(LIBXL_HDTYPE_AHCI));

    printf("libxl_hdtype -- to JSON:\n");
    json_string = libxl_hdtype_to_json(ctx, LIBXL_HDTYPE_IDE);
    printf("\tIDE = %d = %s", LIBXL_HDTYPE_IDE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_hdtype_to_json(ctx, LIBXL_HDTYPE_AHCI);
    printf("\tAHCI = %d = %s", LIBXL_HDTYPE_AHCI, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_hdtype -- from string:\n");
    libxl_hdtype_val = -1;
    rc = libxl_hdtype_from_string("iDe", &libxl_hdtype_val);
    printf("\tIDE = \"%s\" = %d (rc %d)\n", "iDe", libxl_hdtype_val, rc);
    libxl_hdtype_val = -1;
    rc = libxl_hdtype_from_string("ahci", &libxl_hdtype_val);
    printf("\tAHCI = \"%s\" = %d (rc %d)\n", "ahci", libxl_hdtype_val, rc);
    libxl_hdtype_val = -1;
    rc = libxl_hdtype_from_string("An InVaLID ValUE", &libxl_hdtype_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An InVaLID ValUE", libxl_hdtype_val, rc);

    printf("libxl_checkpointed_stream -- to string:\n");
    printf("\tNONE = %d = \"%s\"\n", LIBXL_CHECKPOINTED_STREAM_NONE, libxl_checkpointed_stream_to_string(LIBXL_CHECKPOINTED_STREAM_NONE));
    printf("\tREMUS = %d = \"%s\"\n", LIBXL_CHECKPOINTED_STREAM_REMUS, libxl_checkpointed_stream_to_string(LIBXL_CHECKPOINTED_STREAM_REMUS));
    printf("\tCOLO = %d = \"%s\"\n", LIBXL_CHECKPOINTED_STREAM_COLO, libxl_checkpointed_stream_to_string(LIBXL_CHECKPOINTED_STREAM_COLO));

    printf("libxl_checkpointed_stream -- to JSON:\n");
    json_string = libxl_checkpointed_stream_to_json(ctx, LIBXL_CHECKPOINTED_STREAM_NONE);
    printf("\tNONE = %d = %s", LIBXL_CHECKPOINTED_STREAM_NONE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_checkpointed_stream_to_json(ctx, LIBXL_CHECKPOINTED_STREAM_REMUS);
    printf("\tREMUS = %d = %s", LIBXL_CHECKPOINTED_STREAM_REMUS, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_checkpointed_stream_to_json(ctx, LIBXL_CHECKPOINTED_STREAM_COLO);
    printf("\tCOLO = %d = %s", LIBXL_CHECKPOINTED_STREAM_COLO, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_checkpointed_stream -- from string:\n");
    libxl_checkpointed_stream_val = -1;
    rc = libxl_checkpointed_stream_from_string("NoNe", &libxl_checkpointed_stream_val);
    printf("\tNONE = \"%s\" = %d (rc %d)\n", "NoNe", libxl_checkpointed_stream_val, rc);
    libxl_checkpointed_stream_val = -1;
    rc = libxl_checkpointed_stream_from_string("reMUS", &libxl_checkpointed_stream_val);
    printf("\tREMUS = \"%s\" = %d (rc %d)\n", "reMUS", libxl_checkpointed_stream_val, rc);
    libxl_checkpointed_stream_val = -1;
    rc = libxl_checkpointed_stream_from_string("CoLo", &libxl_checkpointed_stream_val);
    printf("\tCOLO = \"%s\" = %d (rc %d)\n", "CoLo", libxl_checkpointed_stream_val, rc);
    libxl_checkpointed_stream_val = -1;
    rc = libxl_checkpointed_stream_from_string("AN InValiD vALue", &libxl_checkpointed_stream_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN InValiD vALue", libxl_checkpointed_stream_val, rc);

    printf("libxl_vuart_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_VUART_TYPE_UNKNOWN, libxl_vuart_type_to_string(LIBXL_VUART_TYPE_UNKNOWN));
    printf("\tSBSA_UART = %d = \"%s\"\n", LIBXL_VUART_TYPE_SBSA_UART, libxl_vuart_type_to_string(LIBXL_VUART_TYPE_SBSA_UART));

    printf("libxl_vuart_type -- to JSON:\n");
    json_string = libxl_vuart_type_to_json(ctx, LIBXL_VUART_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_VUART_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_vuart_type_to_json(ctx, LIBXL_VUART_TYPE_SBSA_UART);
    printf("\tSBSA_UART = %d = %s", LIBXL_VUART_TYPE_SBSA_UART, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_vuart_type -- from string:\n");
    libxl_vuart_type_val = -1;
    rc = libxl_vuart_type_from_string("unKNOWN", &libxl_vuart_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "unKNOWN", libxl_vuart_type_val, rc);
    libxl_vuart_type_val = -1;
    rc = libxl_vuart_type_from_string("Sbsa_UArt", &libxl_vuart_type_val);
    printf("\tSBSA_UART = \"%s\" = %d (rc %d)\n", "Sbsa_UArt", libxl_vuart_type_val, rc);
    libxl_vuart_type_val = -1;
    rc = libxl_vuart_type_from_string("aN inVaLiD vALUE", &libxl_vuart_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN inVaLiD vALUE", libxl_vuart_type_val, rc);

    printf("libxl_gic_version -- to string:\n");
    printf("\tDEFAULT = %d = \"%s\"\n", LIBXL_GIC_VERSION_DEFAULT, libxl_gic_version_to_string(LIBXL_GIC_VERSION_DEFAULT));
    printf("\tV2 = %d = \"%s\"\n", LIBXL_GIC_VERSION_V2, libxl_gic_version_to_string(LIBXL_GIC_VERSION_V2));
    printf("\tV3 = %d = \"%s\"\n", LIBXL_GIC_VERSION_V3, libxl_gic_version_to_string(LIBXL_GIC_VERSION_V3));

    printf("libxl_gic_version -- to JSON:\n");
    json_string = libxl_gic_version_to_json(ctx, LIBXL_GIC_VERSION_DEFAULT);
    printf("\tDEFAULT = %d = %s", LIBXL_GIC_VERSION_DEFAULT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_gic_version_to_json(ctx, LIBXL_GIC_VERSION_V2);
    printf("\tV2 = %d = %s", LIBXL_GIC_VERSION_V2, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_gic_version_to_json(ctx, LIBXL_GIC_VERSION_V3);
    printf("\tV3 = %d = %s", LIBXL_GIC_VERSION_V3, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_gic_version -- from string:\n");
    libxl_gic_version_val = -1;
    rc = libxl_gic_version_from_string("defAuLt", &libxl_gic_version_val);
    printf("\tDEFAULT = \"%s\" = %d (rc %d)\n", "defAuLt", libxl_gic_version_val, rc);
    libxl_gic_version_val = -1;
    rc = libxl_gic_version_from_string("V2", &libxl_gic_version_val);
    printf("\tV2 = \"%s\" = %d (rc %d)\n", "V2", libxl_gic_version_val, rc);
    libxl_gic_version_val = -1;
    rc = libxl_gic_version_from_string("v3", &libxl_gic_version_val);
    printf("\tV3 = \"%s\" = %d (rc %d)\n", "v3", libxl_gic_version_val, rc);
    libxl_gic_version_val = -1;
    rc = libxl_gic_version_from_string("An inValiD VaLuE", &libxl_gic_version_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An inValiD VaLuE", libxl_gic_version_val, rc);

    printf("libxl_altp2m_mode -- to string:\n");
    printf("\tDISABLED = %d = \"%s\"\n", LIBXL_ALTP2M_MODE_DISABLED, libxl_altp2m_mode_to_string(LIBXL_ALTP2M_MODE_DISABLED));
    printf("\tMIXED = %d = \"%s\"\n", LIBXL_ALTP2M_MODE_MIXED, libxl_altp2m_mode_to_string(LIBXL_ALTP2M_MODE_MIXED));
    printf("\tEXTERNAL = %d = \"%s\"\n", LIBXL_ALTP2M_MODE_EXTERNAL, libxl_altp2m_mode_to_string(LIBXL_ALTP2M_MODE_EXTERNAL));
    printf("\tLIMITED = %d = \"%s\"\n", LIBXL_ALTP2M_MODE_LIMITED, libxl_altp2m_mode_to_string(LIBXL_ALTP2M_MODE_LIMITED));

    printf("libxl_altp2m_mode -- to JSON:\n");
    json_string = libxl_altp2m_mode_to_json(ctx, LIBXL_ALTP2M_MODE_DISABLED);
    printf("\tDISABLED = %d = %s", LIBXL_ALTP2M_MODE_DISABLED, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_altp2m_mode_to_json(ctx, LIBXL_ALTP2M_MODE_MIXED);
    printf("\tMIXED = %d = %s", LIBXL_ALTP2M_MODE_MIXED, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_altp2m_mode_to_json(ctx, LIBXL_ALTP2M_MODE_EXTERNAL);
    printf("\tEXTERNAL = %d = %s", LIBXL_ALTP2M_MODE_EXTERNAL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_altp2m_mode_to_json(ctx, LIBXL_ALTP2M_MODE_LIMITED);
    printf("\tLIMITED = %d = %s", LIBXL_ALTP2M_MODE_LIMITED, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_altp2m_mode -- from string:\n");
    libxl_altp2m_mode_val = -1;
    rc = libxl_altp2m_mode_from_string("dISaBLEd", &libxl_altp2m_mode_val);
    printf("\tDISABLED = \"%s\" = %d (rc %d)\n", "dISaBLEd", libxl_altp2m_mode_val, rc);
    libxl_altp2m_mode_val = -1;
    rc = libxl_altp2m_mode_from_string("MIxed", &libxl_altp2m_mode_val);
    printf("\tMIXED = \"%s\" = %d (rc %d)\n", "MIxed", libxl_altp2m_mode_val, rc);
    libxl_altp2m_mode_val = -1;
    rc = libxl_altp2m_mode_from_string("EXterNaL", &libxl_altp2m_mode_val);
    printf("\tEXTERNAL = \"%s\" = %d (rc %d)\n", "EXterNaL", libxl_altp2m_mode_val, rc);
    libxl_altp2m_mode_val = -1;
    rc = libxl_altp2m_mode_from_string("LIMiTed", &libxl_altp2m_mode_val);
    printf("\tLIMITED = \"%s\" = %d (rc %d)\n", "LIMiTed", libxl_altp2m_mode_val, rc);
    libxl_altp2m_mode_val = -1;
    rc = libxl_altp2m_mode_from_string("AN InvalID VAlue", &libxl_altp2m_mode_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN InvalID VAlue", libxl_altp2m_mode_val, rc);

    printf("libxl_usbctrl_type -- to string:\n");
    printf("\tAUTO = %d = \"%s\"\n", LIBXL_USBCTRL_TYPE_AUTO, libxl_usbctrl_type_to_string(LIBXL_USBCTRL_TYPE_AUTO));
    printf("\tPV = %d = \"%s\"\n", LIBXL_USBCTRL_TYPE_PV, libxl_usbctrl_type_to_string(LIBXL_USBCTRL_TYPE_PV));
    printf("\tDEVICEMODEL = %d = \"%s\"\n", LIBXL_USBCTRL_TYPE_DEVICEMODEL, libxl_usbctrl_type_to_string(LIBXL_USBCTRL_TYPE_DEVICEMODEL));
    printf("\tQUSB = %d = \"%s\"\n", LIBXL_USBCTRL_TYPE_QUSB, libxl_usbctrl_type_to_string(LIBXL_USBCTRL_TYPE_QUSB));

    printf("libxl_usbctrl_type -- to JSON:\n");
    json_string = libxl_usbctrl_type_to_json(ctx, LIBXL_USBCTRL_TYPE_AUTO);
    printf("\tAUTO = %d = %s", LIBXL_USBCTRL_TYPE_AUTO, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_usbctrl_type_to_json(ctx, LIBXL_USBCTRL_TYPE_PV);
    printf("\tPV = %d = %s", LIBXL_USBCTRL_TYPE_PV, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_usbctrl_type_to_json(ctx, LIBXL_USBCTRL_TYPE_DEVICEMODEL);
    printf("\tDEVICEMODEL = %d = %s", LIBXL_USBCTRL_TYPE_DEVICEMODEL, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_usbctrl_type_to_json(ctx, LIBXL_USBCTRL_TYPE_QUSB);
    printf("\tQUSB = %d = %s", LIBXL_USBCTRL_TYPE_QUSB, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_usbctrl_type -- from string:\n");
    libxl_usbctrl_type_val = -1;
    rc = libxl_usbctrl_type_from_string("AUtO", &libxl_usbctrl_type_val);
    printf("\tAUTO = \"%s\" = %d (rc %d)\n", "AUtO", libxl_usbctrl_type_val, rc);
    libxl_usbctrl_type_val = -1;
    rc = libxl_usbctrl_type_from_string("PV", &libxl_usbctrl_type_val);
    printf("\tPV = \"%s\" = %d (rc %d)\n", "PV", libxl_usbctrl_type_val, rc);
    libxl_usbctrl_type_val = -1;
    rc = libxl_usbctrl_type_from_string("dEViCEMoDEl", &libxl_usbctrl_type_val);
    printf("\tDEVICEMODEL = \"%s\" = %d (rc %d)\n", "dEViCEMoDEl", libxl_usbctrl_type_val, rc);
    libxl_usbctrl_type_val = -1;
    rc = libxl_usbctrl_type_from_string("QUSb", &libxl_usbctrl_type_val);
    printf("\tQUSB = \"%s\" = %d (rc %d)\n", "QUSb", libxl_usbctrl_type_val, rc);
    libxl_usbctrl_type_val = -1;
    rc = libxl_usbctrl_type_from_string("An invALiD VaLUE", &libxl_usbctrl_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "An invALiD VaLUE", libxl_usbctrl_type_val, rc);

    printf("libxl_usbdev_type -- to string:\n");
    printf("\tHOSTDEV = %d = \"%s\"\n", LIBXL_USBDEV_TYPE_HOSTDEV, libxl_usbdev_type_to_string(LIBXL_USBDEV_TYPE_HOSTDEV));

    printf("libxl_usbdev_type -- to JSON:\n");
    json_string = libxl_usbdev_type_to_json(ctx, LIBXL_USBDEV_TYPE_HOSTDEV);
    printf("\tHOSTDEV = %d = %s", LIBXL_USBDEV_TYPE_HOSTDEV, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_usbdev_type -- from string:\n");
    libxl_usbdev_type_val = -1;
    rc = libxl_usbdev_type_from_string("HoSTDEv", &libxl_usbdev_type_val);
    printf("\tHOSTDEV = \"%s\" = %d (rc %d)\n", "HoSTDEv", libxl_usbdev_type_val, rc);
    libxl_usbdev_type_val = -1;
    rc = libxl_usbdev_type_from_string("AN INvAlID valUE", &libxl_usbdev_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "AN INvAlID valUE", libxl_usbdev_type_val, rc);

    printf("libxl_event_type -- to string:\n");
    printf("\tDOMAIN_SHUTDOWN = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN));
    printf("\tDOMAIN_DEATH = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_DEATH, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_DEATH));
    printf("\tDISK_EJECT = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DISK_EJECT, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DISK_EJECT));
    printf("\tOPERATION_COMPLETE = %d = \"%s\"\n", LIBXL_EVENT_TYPE_OPERATION_COMPLETE, libxl_event_type_to_string(LIBXL_EVENT_TYPE_OPERATION_COMPLETE));
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = %d = \"%s\"\n", LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE, libxl_event_type_to_string(LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE));

    printf("libxl_event_type -- to JSON:\n");
    json_string = libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN);
    printf("\tDOMAIN_SHUTDOWN = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_DEATH);
    printf("\tDOMAIN_DEATH = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_DEATH, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DISK_EJECT);
    printf("\tDISK_EJECT = %d = %s", LIBXL_EVENT_TYPE_DISK_EJECT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_OPERATION_COMPLETE);
    printf("\tOPERATION_COMPLETE = %d = %s", LIBXL_EVENT_TYPE_OPERATION_COMPLETE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_event_type_to_json(ctx, LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE);
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = %d = %s", LIBXL_EVENT_TYPE_DOMAIN_CREATE_CONSOLE_AVAILABLE, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_event_type -- from string:\n");
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("dOMAIn_sHUtDoWN", &libxl_event_type_val);
    printf("\tDOMAIN_SHUTDOWN = \"%s\" = %d (rc %d)\n", "dOMAIn_sHUtDoWN", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("domAiN_dEATH", &libxl_event_type_val);
    printf("\tDOMAIN_DEATH = \"%s\" = %d (rc %d)\n", "domAiN_dEATH", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("disk_EjECT", &libxl_event_type_val);
    printf("\tDISK_EJECT = \"%s\" = %d (rc %d)\n", "disk_EjECT", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("opERatIon_cOMpLETe", &libxl_event_type_val);
    printf("\tOPERATION_COMPLETE = \"%s\" = %d (rc %d)\n", "opERatIon_cOMpLETe", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("dOMAiN_CreAtE_consOle_AVAilAbLe", &libxl_event_type_val);
    printf("\tDOMAIN_CREATE_CONSOLE_AVAILABLE = \"%s\" = %d (rc %d)\n", "dOMAiN_CreAtE_consOle_AVAilAbLe", libxl_event_type_val, rc);
    libxl_event_type_val = -1;
    rc = libxl_event_type_from_string("aN Invalid VAlUe", &libxl_event_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "aN Invalid VAlUe", libxl_event_type_val, rc);

    printf("libxl_psr_cmt_type -- to string:\n");
    printf("\tCACHE_OCCUPANCY = %d = \"%s\"\n", LIBXL_PSR_CMT_TYPE_CACHE_OCCUPANCY, libxl_psr_cmt_type_to_string(LIBXL_PSR_CMT_TYPE_CACHE_OCCUPANCY));
    printf("\tTOTAL_MEM_COUNT = %d = \"%s\"\n", LIBXL_PSR_CMT_TYPE_TOTAL_MEM_COUNT, libxl_psr_cmt_type_to_string(LIBXL_PSR_CMT_TYPE_TOTAL_MEM_COUNT));
    printf("\tLOCAL_MEM_COUNT = %d = \"%s\"\n", LIBXL_PSR_CMT_TYPE_LOCAL_MEM_COUNT, libxl_psr_cmt_type_to_string(LIBXL_PSR_CMT_TYPE_LOCAL_MEM_COUNT));

    printf("libxl_psr_cmt_type -- to JSON:\n");
    json_string = libxl_psr_cmt_type_to_json(ctx, LIBXL_PSR_CMT_TYPE_CACHE_OCCUPANCY);
    printf("\tCACHE_OCCUPANCY = %d = %s", LIBXL_PSR_CMT_TYPE_CACHE_OCCUPANCY, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cmt_type_to_json(ctx, LIBXL_PSR_CMT_TYPE_TOTAL_MEM_COUNT);
    printf("\tTOTAL_MEM_COUNT = %d = %s", LIBXL_PSR_CMT_TYPE_TOTAL_MEM_COUNT, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cmt_type_to_json(ctx, LIBXL_PSR_CMT_TYPE_LOCAL_MEM_COUNT);
    printf("\tLOCAL_MEM_COUNT = %d = %s", LIBXL_PSR_CMT_TYPE_LOCAL_MEM_COUNT, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_psr_cmt_type -- from string:\n");
    libxl_psr_cmt_type_val = -1;
    rc = libxl_psr_cmt_type_from_string("CAchE_OcCuPANCy", &libxl_psr_cmt_type_val);
    printf("\tCACHE_OCCUPANCY = \"%s\" = %d (rc %d)\n", "CAchE_OcCuPANCy", libxl_psr_cmt_type_val, rc);
    libxl_psr_cmt_type_val = -1;
    rc = libxl_psr_cmt_type_from_string("tOTaL_Mem_count", &libxl_psr_cmt_type_val);
    printf("\tTOTAL_MEM_COUNT = \"%s\" = %d (rc %d)\n", "tOTaL_Mem_count", libxl_psr_cmt_type_val, rc);
    libxl_psr_cmt_type_val = -1;
    rc = libxl_psr_cmt_type_from_string("LocaL_meM_CoUnT", &libxl_psr_cmt_type_val);
    printf("\tLOCAL_MEM_COUNT = \"%s\" = %d (rc %d)\n", "LocaL_meM_CoUnT", libxl_psr_cmt_type_val, rc);
    libxl_psr_cmt_type_val = -1;
    rc = libxl_psr_cmt_type_from_string("an INVAlID VaLue", &libxl_psr_cmt_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an INVAlID VaLue", libxl_psr_cmt_type_val, rc);

    printf("libxl_psr_cbm_type -- to string:\n");
    printf("\tUNKNOWN = %d = \"%s\"\n", LIBXL_PSR_CBM_TYPE_UNKNOWN, libxl_psr_cbm_type_to_string(LIBXL_PSR_CBM_TYPE_UNKNOWN));
    printf("\tL3_CBM = %d = \"%s\"\n", LIBXL_PSR_CBM_TYPE_L3_CBM, libxl_psr_cbm_type_to_string(LIBXL_PSR_CBM_TYPE_L3_CBM));
    printf("\tL3_CBM_CODE = %d = \"%s\"\n", LIBXL_PSR_CBM_TYPE_L3_CBM_CODE, libxl_psr_cbm_type_to_string(LIBXL_PSR_CBM_TYPE_L3_CBM_CODE));
    printf("\tL3_CBM_DATA = %d = \"%s\"\n", LIBXL_PSR_CBM_TYPE_L3_CBM_DATA, libxl_psr_cbm_type_to_string(LIBXL_PSR_CBM_TYPE_L3_CBM_DATA));
    printf("\tL2_CBM = %d = \"%s\"\n", LIBXL_PSR_CBM_TYPE_L2_CBM, libxl_psr_cbm_type_to_string(LIBXL_PSR_CBM_TYPE_L2_CBM));

    printf("libxl_psr_cbm_type -- to JSON:\n");
    json_string = libxl_psr_cbm_type_to_json(ctx, LIBXL_PSR_CBM_TYPE_UNKNOWN);
    printf("\tUNKNOWN = %d = %s", LIBXL_PSR_CBM_TYPE_UNKNOWN, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cbm_type_to_json(ctx, LIBXL_PSR_CBM_TYPE_L3_CBM);
    printf("\tL3_CBM = %d = %s", LIBXL_PSR_CBM_TYPE_L3_CBM, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cbm_type_to_json(ctx, LIBXL_PSR_CBM_TYPE_L3_CBM_CODE);
    printf("\tL3_CBM_CODE = %d = %s", LIBXL_PSR_CBM_TYPE_L3_CBM_CODE, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cbm_type_to_json(ctx, LIBXL_PSR_CBM_TYPE_L3_CBM_DATA);
    printf("\tL3_CBM_DATA = %d = %s", LIBXL_PSR_CBM_TYPE_L3_CBM_DATA, json_string);
    free(json_string);
    json_string = NULL;
    json_string = libxl_psr_cbm_type_to_json(ctx, LIBXL_PSR_CBM_TYPE_L2_CBM);
    printf("\tL2_CBM = %d = %s", LIBXL_PSR_CBM_TYPE_L2_CBM, json_string);
    free(json_string);
    json_string = NULL;

    printf("libxl_psr_cbm_type -- from string:\n");
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("UNknOWN", &libxl_psr_cbm_type_val);
    printf("\tUNKNOWN = \"%s\" = %d (rc %d)\n", "UNknOWN", libxl_psr_cbm_type_val, rc);
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("L3_cBM", &libxl_psr_cbm_type_val);
    printf("\tL3_CBM = \"%s\" = %d (rc %d)\n", "L3_cBM", libxl_psr_cbm_type_val, rc);
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("L3_cBm_cOdE", &libxl_psr_cbm_type_val);
    printf("\tL3_CBM_CODE = \"%s\" = %d (rc %d)\n", "L3_cBm_cOdE", libxl_psr_cbm_type_val, rc);
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("l3_CbM_dATA", &libxl_psr_cbm_type_val);
    printf("\tL3_CBM_DATA = \"%s\" = %d (rc %d)\n", "l3_CbM_dATA", libxl_psr_cbm_type_val, rc);
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("L2_Cbm", &libxl_psr_cbm_type_val);
    printf("\tL2_CBM = \"%s\" = %d (rc %d)\n", "L2_Cbm", libxl_psr_cbm_type_val, rc);
    libxl_psr_cbm_type_val = -1;
    rc = libxl_psr_cbm_type_from_string("an iNVaLiD vaLUE", &libxl_psr_cbm_type_val);
    printf("\tAN INVALID VALUE = \"%s\" = %d (rc %d)\n", "an iNVaLiD vaLUE", libxl_psr_cbm_type_val, rc);



    libxl_ctx_free(ctx);
    xtl_logger_destroy((xentoollog_logger*)logger);

    return 0;
}
