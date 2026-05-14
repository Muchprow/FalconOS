typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// --- ВВОД/ВЫВОД ---
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- ВИДЕО ТЕРМИНАЛА ---
int cursor_x = 0, cursor_y = 0;
const uint8_t THEME_CYBER = 0x0A; // Светло-зеленый на черном (как в Матрице)

void k_print_char(char c, uint8_t col) {
    if (c == '\n') {
        cursor_x = 0; cursor_y++;
    } else {
        volatile char* v = (volatile char*)(0xB8000 + (cursor_y * 80 + cursor_x) * 2);
        *v++ = c; *v = col;
        cursor_x++;
    }
    if (cursor_x >= 80) { cursor_x = 0; cursor_y++; }
}

void k_print(const char* str, uint8_t col) {
    while (*str) k_print_char(*str++, col);
}

void k_clear() {
    for (int i = 0; i < 80 * 25; i++) {
        k_print_char(' ', 0x07);
    }
    cursor_x = 0; cursor_y = 0;
}

// --- ЛОГИКА КОМАНД (CyberSecurity/Admin) ---
void execute_command(char* cmd) {
    k_print("\n", THEME_CYBER);
    
    // Сравнение строк (упрощенное)
    if (cmd[0] == 'n' && cmd[1] == 'e' && cmd[2] == 't') {
        k_print("[OK] Scanning network interfaces...\n", THEME_CYBER);
        k_print("eth0: 192.168.1.105 [UP]\n", 0x02);
        k_print("lo: 127.0.0.1 [UP]\n", 0x02);
    } 
    else if (cmd[0] == 's' && cmd[1] == 'c' && cmd[2] == 'a' && cmd[3] == 'n') {
        k_print("Searching for vulnerabilities...\n", 0x0E);
        k_print("PORT 80: OPEN\nPORT 443: OPEN\nPORT 22: FILTERED\n", 0x0A);
    }
    else if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p') {
        k_print("Available: net, scan, clear, fort_test\n", 0x0F);
    }
    else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r') {
        k_clear();
    }
    else {
        k_print("Unknown command. Type 'help'\n", 0x04); // Красный цвет
    }
    k_print("falcon@root:~# ", 0x0B);
}

// --- КЛАВИАТУРА ---
char cmd_buffer[64];
int cmd_idx = 0;

void keyboard_handler() {
    uint8_t sc = inb(0x60);
    static char map[] = "..1234567890-=\b.qwertyuiop[]\n.asdfghjkl;'`..zxcvbnm,./... ";
    
    if (!(sc & 0x80)) {
        char c = map[sc];
        if (c == '\n') {
            cmd_buffer[cmd_idx] = 0;
            execute_command(cmd_buffer);
            cmd_idx = 0;
        } else if (c == '\b' && cmd_idx > 0) {
            cmd_idx--; cursor_x--;
            k_print_char(' ', 0x07); cursor_x--;
        } else if (cmd_idx < 63 && sc < 58) {
            k_print_char(c, 0x0F);
            cmd_buffer[cmd_idx++] = c;
        }
    }
    outb(0x20, 0x20);
}

// --- IDT И СИСТЕМА ---
struct idt_entry { uint16_t l, s; uint8_t z, f; uint16_t h; } __attribute__((packed));
struct idt_ptr { uint16_t lim; uint32_t base; } __attribute__((packed));
struct idt_entry idt[256]; struct idt_ptr idtp;

void idt_set(uint8_t n, uint32_t base) {
    idt[n].l = base & 0xFFFF; idt[n].h = (base >> 16) & 0xFFFF;
    idt[n].s = 0x08; idt[n].z = 0; idt[n].f = 0x8E;
}

__asm__(".global k_asm\nk_asm: pushal\ncall keyboard_handler\npopal\niret\n"
        ".global t_asm\nt_asm: pushal\nmov $0x20,%al\nout %al,$0x20\npopal\niret\n");

void main() {
    extern void k_asm(), t_asm();
    for(int i=0; i<256; i++) idt_set(i, 0);
    idt_set(32, (uint32_t)t_asm); 
    idt_set(33, (uint32_t)k_asm);
    idtp.lim = 2047; idtp.base = (uint32_t)&idt;
    
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x0);  outb(0xA1, 0x0);
    
    __asm__ volatile("lidt (%0)\nsti" : : "r"(&idtp));

    k_clear();
    k_print("################################\n", 0x02);
    k_print("#       FalconOS v1.0          #\n", 0x02);
    k_print("#   Secure, System and more    #\n", 0x02);
    k_print("################################\n\n", 0x02);
    k_print("System status: SECURE\n", 0x0F);
    k_print("Terminal ready. Type 'help' to begin.\n\n", 0x0F);
    k_print("falcon@root:~# ", 0x0B);

    while(1) __asm__("hlt");
}