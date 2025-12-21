// Various things having to do with the password system - translating them, showing them, entering them, etc.

extern unsigned char currentPassword[5];

void write_state_to_password(void);

void draw_password_screen(void);

void do_password_input(void);

unsigned char read_password_to_state(void);