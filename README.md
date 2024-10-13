### 1. **simpay_list_services**

Fetches a list of SimPay services with pagination.

#### Parameters:

-   `token[]` (string): The API token.
-   `page` (int): The page number to retrieve.
-   `callback[]` (string): The callback function that will be invoked with the results.

#### Callback Signature:

```c
callback(error, Array:data, Trie:pagination);
```

-   `error` (int): Error code. `0` if no error.
-   `data` (Array): Array of service data (ID, name, type, etc.).
-   `pagination` (Trie): Pagination details.

---

### 2. **simpay_service_info**

Fetches information about a specific SimPay service.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service to retrieve.
-   `callback[]` (string): The callback function that will be invoked with the service data.

#### Callback Signature:

```c
callback(error, service_id, id, type, status, name, prefix, suffix, adult, Array:numbers, created_at);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   Other service details: `id`, `type`, `status`, `name`, `prefix`, `suffix`, `adult` (int, 1 if adult), and `created_at`.
-   `numbers` (Array): Array of numbers associated with the service.

---

### 3. **simpay_verify_code**

Verifies an SMS code for a specific service.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service.
-   `code[]` (string): The SMS code to verify.
-   `callback[]` (string): The callback function that will be invoked with the verification result.

#### Callback Signature:

```c
callback(error, service_id, code, used, test, from, number, value, used_at);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   `code` (string): The verified SMS code.
-   `used` (bool): Whether the code was already used.
-   `test` (bool): If it was a test transaction.
-   `from` (string): The sender's phone number.
-   `number` (int): The SMS number.
-   `value` (float): The value of the SMS code.
-   `used_at` (string): The time the code was used (if applicable).

---

### 4. **simpay_transaction_list**

Fetches a list of transactions for a specific service.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service.
-   `page` (int): The page number to retrieve.
-   `callback[]` (string): The callback function that will be invoked with the transaction data.

#### Callback Signature:

```c
callback(error, service_id, Array:data, Trie:pagination);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   `data` (Array): Array of transaction data.
-   `pagination` (Trie): Pagination details.

---

### 5. **simpay_transaction_info**

Fetches detailed information for a specific transaction.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service.
-   `transaction_id[]` (string): The ID of the transaction.
-   `callback[]` (string): The callback function that will be invoked with the transaction data.

#### Callback Signature:

```c
callback(error, service_id, id, from, code, used, send_number, value, send_at);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   `id` (int): The transaction ID.
-   `from` (string): The sender's phone number.
-   `code` (string): The SMS code.
-   `used` (bool): Whether the code was already used.
-   `send_number` (int): The sending number.
-   `value` (float): The value of the transaction.
-   `send_at` (string): The time the transaction was sent.

---

### 6. **simpay_service_numbers**

Fetches a list of numbers associated with a specific service.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service.
-   `page` (int): The page number to retrieve.
-   `callback[]` (string): The callback function that will be invoked with the numbers data.

#### Callback Signature:

```c
callback(error, service_id, Array:data, Trie:pagination);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   `data` (Array): Array of numbers associated with the service.
-   `pagination` (Trie): Pagination details.

---

### 7. **simpay_service_number**

Fetches information about a specific number for a service.

#### Parameters:

-   `token[]` (string): The API token.
-   `service_id[]` (string): The ID of the service.
-   `number` (int): The number to retrieve.
-   `callback[]` (string): The callback function that will be invoked with the number's details.

#### Callback Signature:

```c
callback(error, service_id, number, value, value_gross, adult);
```

-   `error` (int): Error code. `0` if no error.
-   `service_id` (string): The service ID.
-   `number` (int): The number.
-   `value` (float): The net value associated with the number.
-   `value_gross` (float): The gross value.
-   `adult` (bool): Whether the number is adult-only.

---

### Utility Functions

#### **simpay_log_error_message**

Logs an error message based on the error code.

```c
simpay_log_error_message(error);
```

#### **simpay_get_error_message**

Fetches a human-readable error message for a given error code.

```c
simpay_get_error_message(error, out[], len);
```

### Error Codes

-   **SE_CODE_NOT_OK**: HTTP response code not OK.
-   **SE_INVALID_JSON**: Failed to parse JSON from response.
-   **SE_UNSUCCESSFUL**: API response indicated failure.

---
