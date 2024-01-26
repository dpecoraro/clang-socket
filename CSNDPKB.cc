#include <cstring>;
#include "connector.cpp";

extern "C" void CSNDPKB(
    int& return_code,          // By reference to modify the original value
    int& return_reason,        // By reference to modify the original value
    int& rule_array_count,     // By reference, assuming it might be modified or used to pass back information
    const char* rule_array,    // Pointer to char Rule array
    int& private_key_length,   // By reference to return the length of the private key
    char* private_key_value,   // Array to fill with the private key value
    int& key_token_length      // By reference, to return the length of a key token
) {

    return_code = 0; // Success code
    return_reason = 0; // No error

    const char* command = "HSM RSA BUILD CMD";

    const char* dummy_private_key = "dummy_private_key";
    private_key_length = std::strlen(dummy_private_key);
    strncpy(private_key_value, dummy_private_key, private_key_length);
    private_key_value[private_key_length] = '\0'; // Garantindo a terminação nula


    //TODO - Calculate the length of HSM response.
    key_token_length = 0;
    connectToHSM(command);
}