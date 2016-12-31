#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

char *readInputString(FILE* fp, size_t size) {
	char *str;
	int ch;
	size_t len = 0;
	str = (char *)realloc(NULL, sizeof(char)*size);
	if (!str) return str;
	while (EOF != (ch = fgetc(fp)) && ch != '\n') {
		str[len++] = ch;
		if (len == size) {
			str = (char *)realloc(str, sizeof(char)*(size += 16));
			if (!str) return str;
		}
	}
	str[len++] = '\0';

	return (char *)realloc(str, sizeof(char)*len);
}

int strToBinary(char *inputStr, char *binaryStr) {
	int inputSize = 0;
	for (int i = 0; inputStr[i] != '\0'; i++) {
		for (int j = 7; j > -1; j--) {
			binaryStr[(8 * i) + j] = inputStr[i] % 2;
			inputStr[i] /= 2;
		}
		inputSize++;
	}
	binaryStr[inputSize * 8] = '\0';
	return inputSize;
}

void binaryToASCII(char *binaryStr, int *asciiStr, int len) {
	int binaryIndex = 0, n2;

	for (int i = 0; i < len; i++) {
		n2 = 0;
		for (int j = 7; j > -1; j--)
			n2 += binaryStr[binaryIndex++] * pow(2, j);
		asciiStr[i] = n2;
	}
}

int main(void) {
	printf("Enter the message you want to encrypt:\n");

	char* plainStr = readInputString(stdin, 10);
	char binaryStr[1024];

	// Step 1: plain -> binary
	int plainLength = strToBinary(plainStr, binaryStr);
	int binaryLength = plainLength * 8;

	// Step 2: Create a random cipher key the size of the plain text
	char cipherKey[1024];
	int cipherKeyIndex = 0;
	srand(time(NULL)); // The seed

	for (int i = 0; i < binaryLength; i++)
		cipherKey[cipherKeyIndex++] = rand() % 2;

	cipherKey[cipherKeyIndex] = '\0';

	// Step 3: Apply the binary key to our plain Texts binary sequence
	char cipherBinary[1024];
	int cipherBinaryIndex = 0;

	for (int i = 0; i < cipherKeyIndex; i++)
		cipherBinary[cipherBinaryIndex++] = cipherKey[i] ^ binaryStr[i];

	cipherBinary[cipherBinaryIndex] = '\0';

	// Step 4: Convert binary cipher to text
	int cipherASCIIText[1024];
	binaryToASCII(cipherBinary, cipherASCIIText, plainLength);

	// Print the cipher text
	printf("\nThe cipher text is:\n");
	for (int i = 0; i < plainLength; i++) {
		printf("%c", cipherASCIIText[i]);
	}
	printf("\n");

	// Step 5: Generate a 32 Bit cipher key
	char _32cipherKey[32];

	time_t t;
	srand(time(&t));
	rand();

	for (int i = 0; i < 32; i++)
		_32cipherKey[i] = rand() % 2;

	// Step 6: XOR plainBinary with 32 bit key
	char _32cipherBinary[1024];

	for (int i = 0; i < binaryLength; i++)
		_32cipherBinary[i] = _32cipherKey[i % 32] ^ binaryStr[i];

	// Step 7: Print the plain text XOR'd with 32-bits as cipher text
	int _32cipherText[1024];
	binaryToASCII(_32cipherBinary, _32cipherText, plainLength);

	printf("\nThe 32 bit key cipher text is:\n");
	for (int i = 0; i < plainLength; i++) {
		printf("%c", _32cipherText[i]);
	}
	printf("\n");

	// Step 8: Ask the attacker for partial plain-text
	printf("\nHey Intruder do you know of any word that appears in the\nsecret communication between Alice and Bob? ");

	char *guessStr = readInputString(stdin, 10);
	printf("\nAnd would you know the position of that word? ");

	int positionOfGuess;
	scanf_s("%d", &positionOfGuess);

	// Step 9: Convert guessed text to binary
	char guessedBinaryString[1024];
	strToBinary(guessStr, guessedBinaryString);

	// Step 10: XOR guessed Text and cipher text from the given index for 32 bits to retreive the binary key
	char _32guessedBinaryKey[1024];
	int _32guessedBinaryIndex = 0;

	for (int i = 0; i < 32; i++) {
		_32guessedBinaryKey[_32guessedBinaryIndex] = _32cipherBinary[8 * positionOfGuess + i] ^ guessedBinaryString[_32guessedBinaryIndex];
		_32guessedBinaryIndex++;
	}
	_32guessedBinaryKey[_32guessedBinaryIndex] = '\0';

	// Step 11: Apply the guessed 32 bit key to the whole cipher text
	char _32DecipherBinaryGuess[1024];

	for (int i = 0; i < binaryLength; i++)
		_32DecipherBinaryGuess[i] = _32guessedBinaryKey[i % 32] ^ _32cipherBinary[i];

	// Step 12: Print the deciphered Text and hope for the best
	int _32GuessedCipherText[1024];
	binaryToASCII(_32DecipherBinaryGuess, _32GuessedCipherText, plainLength);

	printf("\nThe 32 bit key deciphered is text is:\n");
	for (int i = 0; i < plainLength; i++) {
		printf("%c", _32GuessedCipherText[i]);
	}
	printf("\n");

	// Step 13: XOR guessed Text and cipher text from the given index for 32 bits to retreive the binary key
	_32guessedBinaryIndex = 0;

	for (int i = 0; i < 32; i++) {
		_32guessedBinaryKey[_32guessedBinaryIndex] = cipherBinary[8 * positionOfGuess + i] ^ guessedBinaryString[_32guessedBinaryIndex];
		_32guessedBinaryIndex++;
	}
	_32guessedBinaryKey[_32guessedBinaryIndex] = '\0';

	// Step 14: Apply the guessed 32 bit key to the whole cipher text
	for (int i = 0; i < binaryLength; i++)
		_32DecipherBinaryGuess[i] = _32guessedBinaryKey[i % 32] ^ cipherBinary[i];

	// Step 15: Print the deciphered Text and hope for the best
	binaryToASCII(_32DecipherBinaryGuess, _32GuessedCipherText, plainLength);

	printf("\nApplying the same attack on the variable length key yeilds:\n");
	for (int i = 0; i < plainLength; i++) {
		printf("%c", _32GuessedCipherText[i]);
	}
	printf("\n");

	system("pause");
	getchar();
	return 0;
}