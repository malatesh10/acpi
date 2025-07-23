//Let read acpi table DSDT table from the ACPI table

#include <linux/acpi.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>


#include <linux/module.h>
#include <linux/init.h>
#include <linux/acpi.h>
#include <linux/kernel.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("ACPI driver to locate HDAS device and read _ADR");

static acpi_status hdas_callback(acpi_handle handle, u32 level, void *context, void **retval)
{
    acpi_status status;
    struct acpi_buffer path = { ACPI_ALLOCATE_BUFFER, NULL };
    struct acpi_buffer adr_buf = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *adr_obj;

    pr_info("HDAS ACPI hdas_callback module loaded\n");
    // Get full ACPI path of the current device node
    status = acpi_get_name(handle, ACPI_FULL_PATHNAME, &path);
    if (ACPI_FAILURE(status)) {
        pr_err("Failed to get ACPI path\n");
        return AE_OK; // Continue walking
    } 
    pr_info("ACPI object path: %s | depth level: %u\n", (char *)path.pointer, level);

    // Check if this node is HDAS (case-sensitive match)
    if (strstr((char *)path.pointer, "HDAS")) {
        pr_info("Found HDAS node: %s\n", (char *)path.pointer);

        // Try to evaluate the _ADR method to get device address
        status = acpi_evaluate_object(handle, "_ADR", NULL, &adr_buf);
        if (ACPI_SUCCESS(status)) {
            adr_obj = (union acpi_object *)adr_buf.pointer;
            if (adr_obj && adr_obj->type == ACPI_TYPE_INTEGER) {
                pr_info("HDAS _ADR: 0x%llx\n", adr_obj->integer.value);
            } else {
                pr_info("HDAS _ADR exists but is not an integer\n");
            }
            kfree(adr_buf.pointer);
        } else {
            pr_info("HDAS node has no _ADR method\n");
        }
    }

    kfree(path.pointer);
    return AE_OK; //Continue walking
}

static int __init hdas_probe_init(void)
{
    pr_info("HDAS ACPI probe module loaded\n");

    // Walk the entire ACPI namespace looking for device nodes
    acpi_walk_namespace(ACPI_TYPE_DEVICE,
                        ACPI_ROOT_OBJECT,
                        3,                  // Max depth
                        hdas_callback,    // Callback for each node
                        NULL,             // No predicate
                        NULL,             // No context
                        NULL);            // No return value

    return 0;
}

static void __exit hdas_probe_exit(void)
{
    pr_info("HDAS ACPI probe module unloaded\n");
}

module_init(hdas_probe_init);
module_exit(hdas_probe_exit);