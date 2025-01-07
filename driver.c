#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/input.h>
#include <linux/jiffies.h>

#define PROC_FILENAME "wallpaper"
#define PROC_MAX_SIZE 128

static struct proc_dir_entry *proc_file;
static char proc_data[PROC_MAX_SIZE];
static int click_count = 0;
static unsigned long last_click_time = 0;

static struct input_handler wallpaper_input_handler;
static struct input_dev *input_device;
static ssize_t proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    if (count > PROC_MAX_SIZE)
        count = PROC_MAX_SIZE;

    if (copy_from_user(proc_data, buf, count))
        return -EFAULT;

    proc_data[count] = '\0';
    pr_info("Data written to proc file: %s\n", proc_data);

    return count;
}

static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return simple_read_from_buffer(buf, count, ppos, proc_data, strlen(proc_data));
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static void wallpaper_event(struct input_handle *handle, unsigned int type, unsigned int code, int value)
{
    if (type == EV_KEY && code == BTN_LEFT && value == 1) // Detect left mouse button clicks
    {
        unsigned long current_time = jiffies;

        if (time_before(current_time, last_click_time + msecs_to_jiffies(500)))
        {
            click_count++;
            pr_info("Click count: %d\n", click_count);
        }
        else
        {
            click_count = 1;
        }

        last_click_time = current_time;

        if (click_count == 3)
        {
            snprintf(proc_data, sizeof(proc_data), "change");
            pr_info("Triple click detected. Proc data updated.\n");

            click_count = 0;
        }
    }
}

static int wallpaper_input_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id)
{
    struct input_handle *handle;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "wallpaper_handler";

    input_device = dev; // Save input device reference

    if (input_register_handle(handle))
    {
        kfree(handle);
        return -EINVAL;
    }

    input_open_device(handle);

    pr_info("Connected to input device: %s\n", dev->name);

    return 0;
}

static void wallpaper_input_disconnect(struct input_handle *handle)
{
    pr_info("Disconnected from input device: %s\n", handle->dev->name);

    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id wallpaper_ids[] = {
    {.driver_info = 1},
    {},
};

MODULE_DEVICE_TABLE(input, wallpaper_ids);

static struct input_handler wallpaper_input_handler = {
    .event = wallpaper_event,
    .connect = wallpaper_input_connect,
    .disconnect = wallpaper_input_disconnect,
    .name = "wallpaper_handler",
    .id_table = wallpaper_ids,
};

static int __init proc_click_init(void)
{
    int ret;

    proc_file = proc_create(PROC_FILENAME, 0666, NULL, &proc_fops);
    if (!proc_file)
    {
        pr_err("Failed to create proc file.\n");
        return -ENOMEM;
    }

    snprintf(proc_data, sizeof(proc_data), "Waiting for three consecutive clicks...\n");
    pr_info("Proc file created at /proc/%s\n", PROC_FILENAME);

    ret = input_register_handler(&wallpaper_input_handler);
    if (ret)
    {
        pr_err("Failed to register input handler.\n");
        proc_remove(proc_file);
        return ret;
    }

    return 0;
}

static void __exit proc_click_exit(void)
{
    input_unregister_handler(&wallpaper_input_handler);

    if (proc_file)
        proc_remove(proc_file);

    pr_info("Proc file removed and input handler unregistered.\n");
}

module_init(proc_click_init);
module_exit(proc_click_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Soban Ijaz");
MODULE_DESCRIPTION("Wallpaper Changer on Mouse Click");