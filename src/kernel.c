typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// --- ПОРТЫ ---
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- ВИДЕО ---
void k_print(const char* str, uint8_t color, int x, int y) {
    volatile char* video = (volatile char*)(0xB8000 + (y * 80 + x) * 2);
    while (*str) {
        *video++ = *str++;
        *video++ = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint8_t bg_color) {
    volatile char* video = (volatile char*)0xB8000;
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int offset = (i * 80 + j) * 2;
            video[offset] = ' ';
            video[offset + 1] = (bg_color << 4);
        }
    }
}

// --- IDT ---
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// --- ОБРАБОТЧИКИ ---

void fault_handler() {
    k_print(" !!! CPU PANIC: EXCEPTION !!! ", 0x4F, 25, 0);
    while(1) __asm__("hlt");
}

unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

int cursor_x = 33;

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char c = kbd_us[scancode];
        if (c > 0 && cursor_x < 63) {
            char buf[2] = {c, 0};
            k_print(buf, 0x70, cursor_x++, 13);
        }
    }
    outb(0x20, 0x20);
}

// Ассемблерные заглушки
__asm__(
    ".align 16\n"
    ".global kbd_handler_asm\n"
    "kbd_handler_asm:\n"
    "pushal\n"
    "call keyboard_handler\n"
    "popal\n"
    "iret\n"

    ".global timer_handler_asm\n"
    "timer_handler_asm:\n"
    "pushal\n"
    "mov $0x20, %al\n"
    "out %al, $0x20\n" // Шлем EOI таймеру, чтобы он не вешал систему
    "popal\n"
    "iret\n"

    ".global fault_handler_asm\n"
    "fault_handler_asm:\n"
    "call fault_handler\n"
    "iret\n"
);

// --- ЗАПУСК ---

void pic_remap() {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x0);  outb(0xA1, 0x0);
}

void main() {
    extern void kbd_handler_asm();
    extern void fault_handler_asm();
    extern void timer_handler_asm();

    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)fault_handler_asm, 0x08, 0x8E);
    }

    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)kbd_handler_asm, 0x08, 0x8E);

    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    pic_remap();
    __asm__ volatile("lidt (%0)" : : "r" (&idtp));
    __asm__ volatile("sti");

    outb(0x3D4, 0x0A); outb(0x3D5, 0x20);

    draw_rect(0, 0, 80, 25, 3);
    draw_rect(15, 5, 50, 12, 7);
    draw_rect(15, 5, 50, 1, 1);
    k_print(" SYSTEM CONSOLE ", 0x1F, 16, 5);
    k_print("Welcome to FalconOS!", 0x70, 17, 7);
    k_print("Type something: ", 0x70, 17, 13);

    while (1) {
        __asm__ volatile("hlt");
    }
}