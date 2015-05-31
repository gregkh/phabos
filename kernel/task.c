/*
 * Copyright (C) 2014-2015 Fabien Parent. All rights reserved.
 * Author: Fabien Parent <parent.f@gmail.com>
 *
 * Provided under the three clause BSD license found in the LICENSE file.
 */

#include <asm/irq.h>
#include <asm/spinlock.h>
#include <phabos/scheduler.h>
#include <phabos/utils.h>
#include <phabos/panic.h>

#include <stdlib.h>
#include <errno.h>

#define DEFAULT_STACK_SIZE              4096

static hashtable_t task_table;
static struct spinlock task_table_lock = SPINLOCK_INIT(task_table_lock);
static int next_task_id;

void task_init(void)
{
    hashtable_init_uint(&task_table);
}

static struct task *find_task_by_id(int id)
{
    struct task *task;

    RET_IF_FAIL(id >= 0, NULL);

    spinlock_lock(&task_table_lock);
    task = hashtable_get(&task_table, (void*) id);
    spinlock_unlock(&task_table_lock);

    return task;
}

int _getpid(void)
{
    return current->id;
}

int _kill(int pid, int sig)
{
    struct task *task = find_task_by_id(pid);
    if (!task) {
        errno = ESRCH;
        return -1;
    }

    task_kill(task);

    return 0;
}

void _exit(int code)
{
    // Trying to kill init process
    if (current->id == 0)
        panic("scheduler: trying to exit from idle task.\n");

    task_kill(current);
    panic("scheduler: reach unreachable...\n");
}

struct task *task_create(void)
{
    struct task *task;

    task = zalloc(sizeof(*task));
    RET_IF_FAIL(task, NULL);

    list_init(&task->list);
    hashtable_init_uint(&task->fd);

    irq_disable();
    task->id = next_task_id++;
    irq_enable();

    return task;
}

void task_cond_wait(struct task_cond* cond, struct mutex *mutex)
{
    mutex_unlock(mutex);
    task_add_to_wait_list(task_get_running(), &cond->wait_list);
    mutex_lock(mutex);
}

void task_cond_signal(struct task_cond* cond)
{
    task_remove_from_wait_list(list_first_entry(&cond->wait_list,
                                                struct task, list));
}

void task_cond_broadcast(struct task_cond* cond)
{
    list_foreach_safe(&cond->wait_list, iter)
        task_remove_from_wait_list(list_entry(iter, struct task, list));
}

static void task_destroy(struct task *task)
{
    // assert
    if (task->allocated_stack)
        free(task->allocated_stack);
    free(task);
}

struct task *task_get_running(void)
{
    return current;
}

void task_add_to_wait_list(struct task *task, struct list_head *wait_list)
{
    irq_disable();

    if (task->id == 0)
        panic("PANIC: Trying to remove idle task from runqueue\n");

    list_del(&task->list);
    list_add(wait_list, &task->list);
    task->state &= ~TASK_RUNNING;

    irq_enable();
}

void task_remove_from_wait_list(struct task *task)
{
    irq_disable();

    list_del(&task->list);
    scheduler_add_to_runqueue(task);
    task->state |= TASK_RUNNING;

    irq_enable();
}

struct task *task_run(task_entry_t entry, void *data, uint32_t stack_addr)
{
    struct task *task = task_create();
    if (!task)
        return NULL;

    if (!stack_addr) {
        task->allocated_stack = malloc(DEFAULT_STACK_SIZE);
        if (!task->allocated_stack)
            goto error_stack;

        stack_addr = (uint32_t) task->allocated_stack + DEFAULT_STACK_SIZE;
    }

    task_init_registers(task, entry, data, stack_addr);
    task->state = TASK_RUNNING;

    scheduler_add_to_runqueue(task);

    return task;
error_stack:
    free(task);
    return NULL;
}

void task_kill(struct task *task)
{
    if (task->id == 0) {
        kprintf("Oops: trying to kill idle task...\n");
        return;
    }

    irq_disable();
    if (current == task) {
        irq_enable(); // FIXME: force enable the interrupts in order for
                      // PendSV to work
        task_exit();
        panic("scheduler: reach unreachable...\n");
    }

    list_del(&task->list);
    task_destroy(task);

    irq_enable();
}

void task_exit(void)
{
    kill_task = true;
    task_yield();
}