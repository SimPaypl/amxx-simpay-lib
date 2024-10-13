#if defined _simpay_included
    #endinput
#endif
#define _simpay_included

#include <easy_http>

#define SIMPAY_DEBUG_MODE
#define SIMPAY_API_URL "https://api.simpay.pl"
#define SIMPAY_LOG_PREFIX "[SimPay]"

enum _:SimpayError (+= 1)
{
    SE_CODE_NOT_OK = 1,
    SE_INVALID_JSON,
    SE_UNSUCCESSFUL,
};

simpay_list_services(const token[], page, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s", callback);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms?page=%i", SIMPAY_API_URL, page), "__cb_simpay_list_services", options);
}

public __cb_simpay_list_services(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data, SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data, SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data, SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new Array:output_array = ArrayCreate(1, 1);

    if(ezjson_is_array(data))
    {
        for(new i = 0; i < ezjson_array_get_count(data); i++)
        {
            new EzJSON:service_data = ezjson_array_get_value(data, i);
            new Trie:output_data;

            if(ezjson_is_object(service_data))
            {
                output_data = TrieCreate();

                json_str_to_trie_str(service_data, output_data, "id");
                json_str_to_trie_str(service_data, output_data, "type");
                json_str_to_trie_str(service_data, output_data, "status");
                json_str_to_trie_str(service_data, output_data, "name");
                json_str_to_trie_str(service_data, output_data, "prefix");
                json_str_to_trie_str(service_data, output_data, "suffix");
                json_str_to_trie_str(service_data, output_data, "created_at");
                json_int_to_trie_int(service_data, output_data, "adult");
                json_str_to_trie_str(service_data, output_data, "id");

                ArrayPushCell(output_array, output_data);
            }

            ezjson_free(service_data);
        }
    }

    __invoke_simpay_list_services_cb(user_data, _, output_array, json_pagination_to_trie(json_data));

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_list_services_cb(callback[], error = 0, Array:data = Invalid_Array, Trie:pagination = Invalid_Trie)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_int(any:data);
    callfunc_push_int(any:pagination);
    callfunc_end();
}

simpay_service_info(const token[], const service_id[], callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/%s", SIMPAY_API_URL, service_id), "__cb_simpay_service_info", options);
}

public __cb_simpay_service_info(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));

    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new buffers[7][50];

    ezjson_object_get_string(data, "id", buffers[0], charsmax(buffers[]));
    ezjson_object_get_string(data, "type", buffers[1], charsmax(buffers[]));
    ezjson_object_get_string(data, "status", buffers[2], charsmax(buffers[]));
    ezjson_object_get_string(data, "name", buffers[3], charsmax(buffers[]));
    ezjson_object_get_string(data, "prefix", buffers[4], charsmax(buffers[]));
    ezjson_object_get_string(data, "suffix", buffers[5], charsmax(buffers[]));
    ezjson_object_get_string(data, "created_at", buffers[6], charsmax(buffers[]));

    new EzJSON:json_numbers = ezjson_object_get_value(data, "numbers");
    new adult = ezjson_object_get_number(data, "adult");
    new Array:numbers = ArrayCreate(1, 1);

    json_array_to_dynamic_array(json_numbers, numbers);

    __invoke_simpay_service_info_cb(
        user_data_buffers[0], _, user_data_buffers[1],
        buffers[0], buffers[1], buffers[2], buffers[3], buffers[4], buffers[5],
        adult,
        numbers,
        buffers[6]);

    ezjson_free(json_numbers);
    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_service_info_cb(callback[], error = 0, service_id[] = "", id[] = "", type[] = "", status[] = "", name[] = "", prefix[] = "", suffix[] = "", adult = 0, Array:numbers = Invalid_Array, created_at[] = "")
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_str(id);
    callfunc_push_str(type);
    callfunc_push_str(status);
    callfunc_push_str(name);
    callfunc_push_str(prefix);
    callfunc_push_str(suffix);
    callfunc_push_int(adult);
    callfunc_push_int(any:numbers);
    callfunc_push_str(created_at);
    callfunc_end();
}

simpay_verify_code(const token[], const service_id[], const code[], callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();
    
    new data[100];
    new body[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    new EzJSON:json_body = ezjson_init_object();
    ezjson_object_set_string(json_body, "code", code);
    ezjson_serial_to_string(json_body, body, charsmax(body));
    ezhttp_option_set_body(options, body);

    set_universal_headers(options, token);
    ezhttp_post(fmt("%s/sms/%s", SIMPAY_API_URL, service_id), "__cb_verify_code", options);

    ezjson_free(json_body);
}

public __cb_verify_code(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));
    
    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");

    new code[50], from[50], used_at[50];
    new bool:used = ezjson_object_get_bool(data, "used");
    new bool:test = ezjson_object_get_bool(data, "test");
    new number = ezjson_object_get_number(data, "number");
    new Float:value = json_str_to_float(data, "value");

    ezjson_object_get_string(data, "code", code, charsmax(code));
    ezjson_object_get_string(data, "from", from, charsmax(from));
    ezjson_object_get_string(data, "used_at", used_at, charsmax(used_at));

    __invoke_simpay_verify_code_cb(user_data_buffers[0], _, user_data_buffers[1], code, used, test, from, number, value, used_at);

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_verify_code_cb(callback[], error = 0, service_id[] = "", code[] = "", used = 0, test = 0, from[] = "", number = 0, Float:value = 0.0, used_at[] = "")
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_str(code);
    callfunc_push_int(used);
    callfunc_push_int(test);
    callfunc_push_str(from);
    callfunc_push_int(number);
    callfunc_push_float(value);
    callfunc_push_str(used_at);
    callfunc_end();
}

simpay_transaction_list(const token[], const service_id[], page, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/%s/transactions/?page=%i", SIMPAY_API_URL, service_id, page), "__cb_simpay_transaction_list", options);
}

public __cb_simpay_transaction_list(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));
    
    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new Array:output_array = ArrayCreate(1, 1);

    if(ezjson_is_array(data))
    {
        for(new i = 0; i < ezjson_array_get_count(data); i++)
        {
            new EzJSON:transaction_data = ezjson_array_get_value(data, i);
            new Trie:output_data;
            new number[25];

            if(ezjson_is_object(transaction_data))
            {
                output_data = TrieCreate();

                json_int_to_trie_int(transaction_data, output_data, "id");
                json_str_to_trie_str(transaction_data, output_data, "code");
                json_bool_to_trie_bool(transaction_data, output_data, "used");
                json_str_to_trie_str(transaction_data, output_data, "send_at");

                phone_number_int64_to_str(transaction_data, "from", number);
                TrieSetString(output_data, "from", number);

                ArrayPushCell(output_array, output_data);
            }

            ezjson_free(transaction_data);
        }
    }

    __invoke_simpay_transaction_list_cb(user_data_buffers[0], _, user_data_buffers[1], output_array, json_pagination_to_trie(json_data));

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_transaction_list_cb(callback[], error = 0, service_id[] = "", Array:data = Invalid_Array, Trie:pagination = Invalid_Trie)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_int(any:data);
    callfunc_push_int(any:pagination);
    callfunc_end();
}

simpay_transaction_info(const token[], const service_id[], const transaction_id[], callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/%s/transactions/%s", SIMPAY_API_URL, service_id, transaction_id), "__cb_simpay_transaction_info", options);
}

public __cb_simpay_transaction_info(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));
    
    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new id = ezjson_object_get_number(data, "id");
    new bool:used = ezjson_object_get_bool(data, "used");
    new send_number = ezjson_object_get_number(data, "send_number");
    new Float:value = json_str_to_float(data, "value");

    new from[50], code[50], send_at[50];
    phone_number_int64_to_str(data, "from", from);
    ezjson_object_get_string(data, "code", code, charsmax(code));
    ezjson_object_get_string(data, "send_at", send_at, charsmax(send_at));

    __invoke_simpay_transaction_info_cb(user_data_buffers[0], _, user_data_buffers[1], id, from, code, used, send_number, value, send_at);

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_transaction_info_cb(callback[], error = 0, service_id[] = "", id = 0, from[] = "", code[] = "", used = 0, send_number = 0, Float:value = 0.0, send_at[] = "")
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_int(id);
    callfunc_push_str(from);
    callfunc_push_str(code);
    callfunc_push_int(used);
    callfunc_push_int(send_number);
    callfunc_push_float(value);
    callfunc_push_str(send_at);
    callfunc_end();
}

simpay_service_numbers(const token[], const service_id[], page, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/%s/numbers?page=%i", SIMPAY_API_URL, service_id, page), "__cb_simpay_service_numbers", options);
}

public __cb_simpay_service_numbers(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));
    
    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new Array:output_array = ArrayCreate(1, 1);

    if(ezjson_is_array(data))
    {
        for(new i = 0; i < ezjson_array_get_count(data); i++)
        {
            new EzJSON:number_data = ezjson_array_get_value(data, i);
            new Trie:output_data;

            if(ezjson_is_object(number_data))
            {
                output_data = TrieCreate();

                json_int_to_trie_int(number_data, output_data, "number");
                json_str_to_trie_int(number_data, output_data, "value");
                json_float_to_trie_float(number_data, output_data, "value_gross");
                json_bool_to_trie_bool(number_data, output_data, "adult");

                ArrayPushCell(output_array, output_data);
            }

            ezjson_free(number_data);
        }
    }

    __invoke_simpay_service_numbers(user_data_buffers[0], _, user_data_buffers[1], output_array, json_pagination_to_trie(json_data));

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_service_numbers(callback[], error = 0, service_id[] = "", Array:data = Invalid_Array, Trie:pagination = Invalid_Trie)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_int(any:data);
    callfunc_push_int(any:pagination);
    callfunc_end();
}

simpay_service_number(const token[], const service_id[], number, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s#%s", callback, service_id);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/%s/numbers/%i", SIMPAY_API_URL, service_id, number), "__cb_simpay_service_number", options);
}

public __cb_simpay_service_number(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    new user_data_buffers[2][50];
    explode_string(user_data, "#", user_data_buffers, 2, sizeof(user_data_buffers[]));
    
    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data_buffers[0], SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new number = ezjson_object_get_number(data, "number");
    new Float:value = json_str_to_float(data, "value");
    new Float:value_gross = ezjson_object_get_real(data, "value_gross");
    new bool:adult = ezjson_object_get_bool(data, "adult");

    __invoke_simpay_service_number_cb(user_data_buffers[0], _, user_data_buffers[1], number, value, value_gross, adult);

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_service_number_cb(callback[], error = 0 , service_id[] = "", number = 0, Float:value = 0.0, Float:value_gross = 0.0, adult = 0)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_str(service_id);
    callfunc_push_int(number);
    callfunc_push_float(value);
    callfunc_push_float(value_gross);
    callfunc_push_int(adult);
    callfunc_end();
}

simpay_all_numbers(const token[], page, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s", callback);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/numbers?page=%i", SIMPAY_API_URL, page), "__cb_simpay_all_numbers", options);
}

public __cb_simpay_all_numbers(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data, SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data, SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data, SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new Array:output_array = ArrayCreate(1, 1);

    if(ezjson_is_array(data))
    {
        for(new i = 0; i < ezjson_array_get_count(data); i++)
        {
            new EzJSON:number_data = ezjson_array_get_value(data, i);
            new Trie:output_data;

            if(ezjson_is_object(number_data))
            {
                output_data = TrieCreate();

                json_int_to_trie_int(number_data, output_data, "number");
                json_str_to_trie_float(number_data, output_data, "value");
                json_float_to_trie_float(number_data, output_data, "value_gross");
                json_bool_to_trie_bool(number_data, output_data, "adult");

                ArrayPushCell(output_array, output_data);
            }

            ezjson_free(number_data);
        }
    }

    __invoke_simpay_all_numbers_cb(user_data, _, output_array, json_pagination_to_trie(json_data));

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_all_numbers_cb(callback[], error = 0, Array:data = Invalid_Array, Trie:pagination = Invalid_Trie)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_int(any:data);
    callfunc_push_int(any:pagination);
    callfunc_end();
}

simpay_number(const token[], number, callback[])
{
    new EzHttpOptions:options = ezhttp_create_options();

    new data[100];
    formatex(data, charsmax(data), "%s", callback);
    ezhttp_option_set_user_data(options, data, strlen(data));

    set_universal_headers(options, token);
    ezhttp_get(fmt("%s/sms/numbers/%i", SIMPAY_API_URL, number), "__cb_simpay_number", options);
}

public __cb_simpay_number(EzHttpRequest:request_id)
{
    new user_data[100];
    ezhttp_get_user_data(request_id, user_data);

    if(!is_response_code_ok(request_id))
    {
        __invoke_simpay_list_services_cb(user_data, SE_CODE_NOT_OK);

        return;
    }

    new EzJSON:json_data = request_data_to_json(request_id);
    if(json_data == EzInvalid_JSON)
    {
        __invoke_simpay_list_services_cb(user_data, SE_INVALID_JSON);
        ezjson_free(json_data);

        return;
    }

    if(!valid_response(json_data))
    {
        __invoke_simpay_list_services_cb(user_data, SE_UNSUCCESSFUL);
        ezjson_free(json_data);

        return;
    }

    new EzJSON:data = ezjson_object_get_value(json_data, "data");
    new number = ezjson_object_get_number(data, "number");
    new Float:value_gross = ezjson_object_get_real(data, "value_gross");
    new Float:value = json_str_to_float(data, "value");
    new bool:adult = ezjson_object_get_bool(data, "adult");

    __invoke_simpay_number_cb(user_data, _, number, value, value_gross, adult);

    ezjson_free(data);
    ezjson_free(json_data);
}

__invoke_simpay_number_cb(callback[], error = 0, number = 0, Float:value = 0.0, Float:value_gross = 0.0, adult = 0)
{
    callfunc_begin(callback);
    callfunc_push_int(error);
    callfunc_push_int(number);
    callfunc_push_float(value);
    callfunc_push_float(value_gross);
    callfunc_push_int(adult);
    callfunc_end();
}

// Utils
simpay_log_error_message(error)
{
    new error_message[100];
    simpay_get_erorr_message(error, error_message, charsmax(error_message));

    log_amx("%s Error %i: %s", SIMPAY_LOG_PREFIX, error, error_message);
}

simpay_get_erorr_message(error, out[], len)
{
    switch(error)
    {
        case SE_CODE_NOT_OK: formatex(out, len, "HTTP code not ok");
        case SE_INVALID_JSON: formatex(out, len, "Could not parse json");
        case SE_UNSUCCESSFUL: formatex(out, len, "Unsuccessful response");
        default: formatex(out, len, "Unknown error");
    }
}

Trie:json_pagination_to_trie(EzJSON:json)
{
    new Trie:pagination;
    pagination = TrieCreate();

    TrieSetCell(pagination, "total", ezjson_object_get_number(json, "pagination.total", true));
    TrieSetCell(pagination, "count", ezjson_object_get_number(json, "pagination.count", true));
    TrieSetCell(pagination, "per_page", ezjson_object_get_number(json, "pagination.per_page", true));
    TrieSetCell(pagination, "current_page", ezjson_object_get_number(json, "pagination.current_page", true));
    TrieSetCell(pagination, "total_pages", ezjson_object_get_number(json, "pagination.total_pages", true));

    new next_page[200];
    ezjson_object_get_string(json, "pagination.links.next_page", next_page, charsmax(next_page), true);
    TrieSetString(pagination, "next_page", next_page);

    new prev_page[200];
    ezjson_object_get_string(json, "pagination.links.prev_page", prev_page, charsmax(prev_page), true);
    TrieSetString(pagination, "prev_page", prev_page);

    return pagination;
}

phone_number_int64_to_str(EzJSON:json, key[], out[])
{
    new buffer[300];
    ezjson_serial_to_string(json, buffer, charsmax(buffer));

    new searched[30];
    formatex(searched, charsmax(searched), "^"%s^":", key);

    new start_at = strfind(buffer, searched);
    new end_at = strfind(buffer, ",", _, start_at);
    
    formatex(out, end_at - start_at - strlen(searched), buffer[start_at + strlen(searched)]);
}

json_array_to_dynamic_array(EzJSON:json, Array:array)
{
    if(ezjson_is_array(json))
    {
        new value[15];

        for(new i = 0; i < ezjson_array_get_count(json); i++)
        {
            ezjson_array_get_string(json, i, value, charsmax(value));
            ArrayPushCell(array, str_to_num(value));
        }
    }
}

Float:json_str_to_float(EzJSON:json, key[])
{
    new value_as_str[20];
    ezjson_object_get_string(json, key, value_as_str, charsmax(value_as_str), true);

    return str_to_float(value_as_str);
}

json_str_to_trie_str(EzJSON:json, Trie:trie, key[])
{
    new str[50];
    ezjson_object_get_string(json, key, str, sizeof(str));
    TrieSetString(trie, key, str);
}

json_bool_to_trie_bool(EzJSON:json, Trie:trie, key[])
{
    TrieSetCell(trie, key, ezjson_object_get_bool(json, key));
}

json_int_to_trie_int(EzJSON:json, Trie:trie, key[])
{
    TrieSetCell(trie, key, ezjson_object_get_number(json, key));
}

json_float_to_trie_float(EzJSON:json, Trie:trie, key[])
{
    TrieSetCell(trie, key, ezjson_object_get_real(json, key));
}

json_str_to_trie_float(EzJSON:json, Trie:trie, key[])
{
    new value_as_str[20];
    ezjson_object_get_string(json, key, value_as_str, charsmax(value_as_str));
    TrieSetCell(trie, key, str_to_float(value_as_str));
}

json_str_to_trie_int(EzJSON:json, Trie:trie, key[])
{
    new value_str[30];
    ezjson_object_get_string(json, key, value_str, charsmax(value_str));
    TrieSetCell(trie, key, str_to_num(value_str));
}

set_universal_headers(EzHttpOptions:options, const token[])
{
    ezhttp_option_set_header(options, "Content-Type", "application/json");
    ezhttp_option_set_header(options, "Authorization", fmt("Bearer %s", token));
}

bool:valid_response(EzJSON:json_data)
{
    new bool:valid = ezjson_object_get_bool(json_data, "success");

    #if defined SIMPAY_DEBUG_MODE
        if(!valid)
        {
            new message[200];
            ezjson_object_get_string(json_data, "message", message, charsmax(message));

            log_amx("%s Unsuccessful response: %s", SIMPAY_LOG_PREFIX, message);
        }
    #endif

    return valid;
}

EzJSON:request_data_to_json(EzHttpRequest:request_id)
{
    new EzJSON:json_data = ezhttp_parse_json_response(request_id);

    if(json_data == EzInvalid_JSON)
    {
        ezjson_free(json_data);

        #if defined SIMPAY_DEBUG_MODE
            log_amx("%s Error converting response data into json.", SIMPAY_LOG_PREFIX);
        #endif
    }

    return json_data;
}

bool:is_response_code_ok(EzHttpRequest:request_id)
{
    if(ezhttp_get_error_code(request_id) != EZH_OK)
    {
        #if defined SIMPAY_DEBUG_MODE
            new error[64];
            ezhttp_get_error_message(request_id, error, charsmax(error));

            log_amx("%s Response error: %s", SIMPAY_LOG_PREFIX, error);
        #endif

        return false;
    }

    return true;
}