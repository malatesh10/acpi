#include <linux/module.h>
#include <linux/init.h>
#include <linux/acpi.h>
#include <linux/kernel.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("ACPI probe for SNDW device");

/**
 * sndw_callback - Callback function for ACPI namespace walk
 * @handle:   ACPI handle of the current object
 * @level:    Depth level of the current object
 * @context:  Context pointer (not used in this function)
 * @retval:   Return value pointer (not used in this function)
 *
 * This function is called for each object in the ACPI namespace during the walk.
 * It checks if the object is a SNDW device and prints its _ADR and _STA values.
 *
 * Return: AE_OK on success, AE_ERROR on failure
 */
static acpi_status sndw_callback(acpi_handle handle, u32 level, void *context, void **retval)
{
    acpi_status status;
    struct acpi_buffer path = { ACPI_ALLOCATE_BUFFER, NULL }; /**< Buffer to store the ACPI path */
    struct acpi_buffer adr_buf = { ACPI_ALLOCATE_BUFFER, NULL }; /**< Buffer to store the _ADR value */
    struct acpi_buffer sta_buf = { ACPI_ALLOCATE_BUFFER, NULL }; /**< Buffer to store the _STA value */
    union acpi_object *obj; /**< Pointer to the ACPI object */

    /**
     * Get the full ACPI path of the current object.
     * If the path is too long, it will be truncated.
     */
    status = acpi_get_name(handle, ACPI_FULL_PATHNAME, &path);
    if (ACPI_FAILURE(status)) {
        /**
         * If the path cannot be retrieved, skip this object and continue with the next one.
         */
        return AE_OK;
    }

    /**
     * Print the ACPI path and depth level of the current object.
     */
    pr_info("ACPI object path: %s | depth level: %u\n", (char *)path.pointer, level);

    /**
     * Check if the object is a SNDW device by searching for the "SNDW" string in the path.
     */
    if (strstr((char *)path.pointer, "SNDW")) {
        pr_info("Found SNDW device: %s\n", (char *)path.pointer);

        /**
         * Evaluate the _ADR method to get the device address.
         */
        status = acpi_evaluate_object(handle, "_ADR", NULL, &adr_buf);
        if (ACPI_SUCCESS(status)) {
            obj = (union acpi_object *)adr_buf.pointer;
            if (obj && obj->type == ACPI_TYPE_INTEGER) {
                /**
                 * Print the _ADR value if it is an integer.
                 */
                pr_info("SNDW _ADR: 0x%llx\n", obj->integer.value);
            }
            /**
             * Free the buffer allocated for the _ADR value.
             */
            kfree(adr_buf.pointer);
        }

        /**
         * Evaluate the _STA method to get the device status.
         */
        status = acpi_evaluate_object(handle, "_STA", NULL, &sta_buf);
        if (ACPI_SUCCESS(status)) {
            obj = (union acpi_object *)sta_buf.pointer;
            if (obj && obj->type == ACPI_TYPE_INTEGER) {
                /**
                 * Print the _STA value if it is an integer.
                 */
                pr_info("SNDW _STA: 0x%llx\n", obj->integer.value);
            }
            /**
             * Free the buffer allocated for the _STA value.
             */
            kfree(sta_buf.pointer);
        }
    }

    /**
     * Free the buffer allocated for the ACPI path.
     */
    kfree(path.pointer);
    return AE_OK;
}

static int __init sndw_probe_init(void)
{
    pr_info("SNDW ACPI probe loaded\n");
    acpi_walk_namespace(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT, 5,
                        sndw_callback, NULL, NULL, NULL);
    return 0;
}

static void __exit sndw_probe_exit(void)
{
    pr_info("SNDW ACPI probe unloaded\n");
}

module_init(sndw_probe_init);
module_exit(sndw_probe_exit);
