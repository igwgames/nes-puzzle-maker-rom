#define PRG_BANK_TEXT_INPUT 2


// Solicits the user for text input. Reads/writes inputText
void do_text_input(const unsigned char* name, const unsigned char maxLength);

extern unsigned char inputText[32];