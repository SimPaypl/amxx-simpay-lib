#include <amxmodx>
#include <simpay>

#define AUTHOR "Robin - github.com/NotRobiin"

public plugin_init()
{
    register_plugin("", "v1.0", AUTHOR);

    register_concmd("simpay_list_services", "list_services");
    register_concmd("simpay_service_info", "service_info");
    register_concmd("simpay_verify_code", "verify_code");
    register_concmd("simpay_transaction_list", "transaction_list");
    register_concmd("simpay_transaction_info", "transaction_info");
    register_concmd("simpay_service_numbers", "service_numbers");
    register_concmd("simpay_service_number", "service_number");
    register_concmd("simpay_all_numbers", "all_numbers");
    register_concmd("simpay_number", "number");
}

// Lista uslug
public list_services()
{
    simpay_list_services("<token>", 3, "cb_list_services");
}

public cb_list_services(error, Array:data, Trie:pagination)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    for(new i = 0; i < ArraySize(data); i++)
    {
        new Trie:service = ArrayGetCell(data, i);
        new adult;
        new id[30], type[30], status[30], name[30], prefix[30], suffix[30], created_at[30];

        TrieGetString(service, "id", id, charsmax(id));
        TrieGetString(service, "type", type, charsmax(type));
        TrieGetString(service, "status", status, charsmax(status));
        TrieGetString(service, "name", name, charsmax(name));
        TrieGetString(service, "prefix", prefix, charsmax(prefix));
        TrieGetString(service, "suffix", suffix, charsmax(suffix));
        TrieGetCell(service, "adult", adult);
        TrieGetString(service, "created_at", created_at, charsmax(created_at));

        log_amx("Service { id=^"%s^" type=^"%s^" status=^"%s^" name=^"%s^" prefix=^"%s^" suffix=^"%s^" adult=^"%s^" created at=^"%s^" }",
        id, type, status, name, prefix, suffix, adult ? "true" : "false", created_at);
    
        TrieDestroy(service);
    }

    new total, count, per_page, current_page, total_pages;
    new next_page[100], prev_page[100];

    TrieGetCell(pagination, "total", total);
    TrieGetCell(pagination, "count", count);
    TrieGetCell(pagination, "per_page", per_page);
    TrieGetCell(pagination, "current_page", current_page);
    TrieGetCell(pagination, "total_pages", total_pages);
    TrieGetString(pagination, "next_page", next_page, charsmax(next_page));
    TrieGetString(pagination, "prev_page", prev_page, charsmax(prev_page));

    log_amx("Pagination: { total=^"%i^" count=^"%i^" per page=^"%i^" current page=^"%i^" total pages=^"%i^" next page=^"%s^" prev page=^"%s^" }",
    total, count, per_page, current_page, total_pages, next_page, prev_page);

    ArrayDestroy(data);
}

// Pobieranie informacji o usludze
public service_info()
{
    simpay_service_info("<token>", "<serviceid>", "cb_service_info");
}

public cb_service_info(error, service_id[], id[], type[], status[], name[], prefix[], suffix[], bool:adult, Array:numbers, created_at[])
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    new numbers_as_str[100];
    for(new i = 0; i < ArraySize(numbers); i++)
    {
        add(numbers_as_str, charsmax(numbers_as_str), fmt("%i ", ArrayGetCell(numbers, i)));
    }

    log_amx("Service info: { id=^"%s^" type=^"%s^" status=^"%s^" name=^"%s^" prefix=^"%s^" suffix=^"%s^" adult=^"%s^" numbers=^"%s^" created at=^"%s^" }",
    id, type, status, name, prefix, suffix, adult ? "true" : "false", numbers_as_str, created_at);
}

// Weryfikacja poprawnosci kodu
public verify_code()
{
    simpay_verify_code("<token>", "<serviceid>", "F0ED7B", "cb_verify_code");
}

public cb_verify_code(error, service_id[], code[], bool:used, bool:test, from[], number, Float:value, used_at[])
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    log_amx("Code verification: { service=^"%s^" code=^"%s^" used=^"%s^" test=^"%s^" from=^"%s^" number=^"%i^" value=^"%0.2f^" used at=^"%s^" }",
    service_id, code, used ? "true" : "false", test ? "true" : "false", from, number, value, used_at);
}

// Pobieranie listy transakcji
public transaction_list()
{
    simpay_transaction_list("<token>", "<serviceid>", 5, "cb_transaction_list");
}

public cb_transaction_list(error, service_id[], Array:data, Trie:pagination)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    for(new i = 0; i < ArraySize(data); i++)
    {
        new Trie:transaction = ArrayGetCell(data, i);
        new id, used;
        new from[30], code[30], send_at[30];

        TrieGetCell(transaction, "id", id);
        TrieGetString(transaction, "from", from, charsmax(from));
        TrieGetString(transaction, "code", code, charsmax(code));
        TrieGetCell(transaction, "used", used);
        TrieGetString(transaction, "send_at", send_at, charsmax(send_at));

        log_amx("{ id=^"%i^" from=^"%s^" code=^"%s^" used=^"%s^" send at=^"%s^" }",
            id, from, code, used ? "true" : "false", send_at);
    
        TrieDestroy(transaction);
    }

    new total, count, per_page, current_page, total_pages;
    new next_page[100], prev_page[100];

    TrieGetCell(pagination, "total", total);
    TrieGetCell(pagination, "count", count);
    TrieGetCell(pagination, "per_page", per_page);
    TrieGetCell(pagination, "current_page", current_page);
    TrieGetCell(pagination, "total_pages", total_pages);
    TrieGetString(pagination, "next_page", next_page, charsmax(next_page));
    TrieGetString(pagination, "prev_page", prev_page, charsmax(prev_page));

    log_amx("Pagination: { total=^"%i^" count=^"%i^" per page=^"%i^" current page=^"%i^" total pages=^"%i^" next page=^"%s^" prev page=^"%s^" }",
    total, count, per_page, current_page, total_pages, next_page, prev_page);

    ArrayDestroy(data);
}

// Pobieranie informacji o transakcji
public transaction_info()
{
    simpay_transaction_info("<token>", "<serviceid>", "<transactionid>", "cb_transaction_info");
}

public cb_transaction_info(error, service_id[], id, from[], code[], bool:used, send_number, Float:value, send_at[])
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    log_amx("Transaction info: { service id=^"%s^" id=^"%i^" from=^"%s^" code=^"%s^" used=^"%s^" send_number=^"%i^" value=^"%0.2f^" send_at=^"%s^" }",
    service_id, id, from, code, used ? "true" : "false", send_number, value, send_at);
}

// Pobieranie informacji o numerach dla uslugi
public service_numbers()
{
    simpay_service_numbers("<token>", "<serviceid>", 2, "cb_service_numbers");
}

public cb_service_numbers(error, service_id[], Array:data, Trie:pagination)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    for(new i = 0; i < ArraySize(data); i++)
    {
        new Trie:service_number = ArrayGetCell(data, i);
        new number, value, adult;
        new Float:value_gross;

        TrieGetCell(service_number, "number", number);
        TrieGetCell(service_number, "value", value);
        TrieGetCell(service_number, "value_gross", value_gross);
        TrieGetCell(service_number, "adult", adult);

        log_amx("{ number=^"%i^" value=^"%i^" value_gross=^"%0.2f^" adult=^"%s^"  }",
            number, value, value_gross, adult ? "true" : "false");
    
        TrieDestroy(service_number);
    }

    new total, count, per_page, current_page, total_pages;
    new next_page[100], prev_page[100];

    TrieGetCell(pagination, "total", total);
    TrieGetCell(pagination, "count", count);
    TrieGetCell(pagination, "per_page", per_page);
    TrieGetCell(pagination, "current_page", current_page);
    TrieGetCell(pagination, "total_pages", total_pages);
    TrieGetString(pagination, "next_page", next_page, charsmax(next_page));
    TrieGetString(pagination, "prev_page", prev_page, charsmax(prev_page));

    log_amx("Pagination: { total=^"%i^" count=^"%i^" per page=^"%i^" current page=^"%i^" total pages=^"%i^" next page=^"%s^" prev page=^"%s^" }",
    total, count, per_page, current_page, total_pages, next_page, prev_page);

    ArrayDestroy(data);
}

// Pobieranie informacji o numerze dla uslugi
public service_number()
{
    simpay_service_number("<token>", "<serviceid>", 91955, "cb_service_number");
}

public cb_service_number(error, service_id[], number, Float:value, Float:value_gross, bool:adult)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    log_amx("Service number: { service id=^"%s^" number=^"%i^" value=^"%0.2f^" value_gross=^"%0.2f^" adult=^"%s^" }",
    service_id, number, value, value_gross, adult ? "true" : "false" );
}

// Pobieranie wszystkich dostepnych numerow
public all_numbers()
{
    simpay_all_numbers("<token>", 1, "cb_all_numbers");
}

public cb_all_numbers(error, Array:data, Trie:pagination)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    for(new i = 0; i < ArraySize(data); i++)
    {
        new Trie:service_number = ArrayGetCell(data, i);
        new number, value, adult;
        new Float:value_gross;

        TrieGetCell(service_number, "number", number);
        TrieGetCell(service_number, "value", value);
        TrieGetCell(service_number, "value_gross", value_gross);
        TrieGetCell(service_number, "adult", adult);

        log_amx("{ number=^"%i^" value=^"%0.2f^" value_gross=^"%0.2f^" adult=^"%s^"  }",
            number, value, value_gross, adult ? "true" : "false");
    
        TrieDestroy(service_number);
    }

    new total, count, per_page, current_page, total_pages;
    new next_page[100], prev_page[100];

    TrieGetCell(pagination, "total", total);
    TrieGetCell(pagination, "count", count);
    TrieGetCell(pagination, "per_page", per_page);
    TrieGetCell(pagination, "current_page", current_page);
    TrieGetCell(pagination, "total_pages", total_pages);
    TrieGetString(pagination, "next_page", next_page, charsmax(next_page));
    TrieGetString(pagination, "prev_page", prev_page, charsmax(prev_page));

    log_amx("Pagination: { total=^"%i^" count=^"%i^" per page=^"%i^" current page=^"%i^" total pages=^"%i^" next page=^"%s^" prev page=^"%s^" }",
    total, count, per_page, current_page, total_pages, next_page, prev_page);

    ArrayDestroy(data);
}

// Pobieranie numeru
public number()
{
    simpay_number("<token>", 7055, "cb_number");
}

public cb_number(error, number, Float:value, Float:value_gross, bool:adult)
{
    if(error)
    {
        simpay_log_error_message(error);
        return;
    }

    log_amx("Number: { number=^"%i^" value=^"%0.2f^" value_gross=^"%0.2f^" adult=^"%s^" }",
        number, value, value_gross, adult ? "true" : "false" );
}