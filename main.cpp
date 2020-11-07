#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>


class DbInterface
{
public:
    DbInterface(std::string path_to_file);
    ~DbInterface();

protected:
    // creates a pointer to the sqlite3 object
    sqlite3* db;

    //Executes a compiled sqlite3 statement
    bool ExecuteQueryNoResultsBack(sqlite3_stmt* my_statement);
    //Compiles a sqlite3 statement from a query string and then executes it
    bool ExecuteQueryNoResultsBack(std::string sql);
    //Creates a compiled sqlite3 statement but does not execute it
    bool PrepareQueryWithResults(std::string sql, sqlite3_stmt*& my_statement);
};


class ManagerInterface : DbInterface
{
public:
    ManagerInterface(std::string path_to_file);
    //Inserts an item in table Items
    void InsertItem(std::string itemName, int price, int quantity, double tax);
    //Displays all the items in the table Items
    void DisplayItems();

    //selects a given row from the Items table
    int SelectItem();
    //following functions updates a selected item from the Items table
    void UpdatePrice();
    void UpdateTax();
    void UpdateQuantity(char flag);
    //Add items to bill then checkout
    void Billing();
    //Delete an item
    void DeleteItem();
    //print the column names
    void PrintIndex();

private:
    //creates Items table
    void CreateItemsTable();
};

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Capitalize(std::string& str);
int CheckedInputInt(std::string err_msg);
double CheckedInputDouble(std::string err_msg);
template<typename T> void printE(T t, int width);



//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


int main()
{
    //creates a ManagerInterface object
    ManagerInterface db("example.db");
    int option;
    int selected_option = -1;

    while(true)
    {
        std::cout << "1. Add an item" << std::endl;
        std::cout << "2. Display all items" << std::endl;
        std::cout << "3. Update an item" << std::endl;
        std::cout << "4. Billing" << std::endl;
        std::cout << "5. Quit" << std::endl;
        std::cout << std::endl;

        std::cout << "Enter an option: " << std::endl;
        //gets an option from user
        option = CheckedInputInt("Option must be");
        std::cout << std::endl;
        //Adds an item
        if (option == 1)
        {
            std::string itemName;
            int price = -1;
            int quantity = -1;
            double tax = -1;


            std::cout << "Enter the item's name: ";
            //gets item's name from user
            std::cin.ignore(); //prevents getline from skipping input 
            std::getline(std::cin, itemName);
            Capitalize(itemName);
            
            //gets item's price from user
            while (price < 1)
            {
                std::cout << "Enter the item's price: ";
                price = CheckedInputInt("price must be");
            }
            //gets item's quantity from user
            while(quantity < 0)
            {
                std::cout << "Enter the item's quantity: ";
                quantity = CheckedInputInt("Quantity must be");
            }
            //gets item's tax from user
            while (tax < 0)
            {
                std::cout << "Enter the item's tax: ";
                tax = CheckedInputDouble("tax must be");
            }

            //Inserts the item in the database
            db.InsertItem(itemName, price, quantity, tax);
            std::cout << std::endl << std::endl;
        }

        //Display all items
        else if (option == 2)
        {
            db.DisplayItems();
        }
        //Update an item
        else if (option == 3)
        {   
            while (true)
            {
                std::cout << "1. Update price" << std::endl;
                std::cout << "2. Update tax" << std::endl;
                std::cout << "3. Update quantity" << std::endl;
                std::cout << "4. Delete an item" << std::endl;
                std::cout << "5. Go back" << std::endl;
                std::cout << std::endl;

                std::cout << "Enter an option: ";
                int update_option;
                //gets option from user
                update_option = CheckedInputInt("Option must be");

                if (update_option == 1)
                {
                    db.UpdatePrice();
                }

                else if (update_option == 2)
                {
                    db.UpdateTax();
                }
                else if (update_option == 3)
                {
                    while (true)
                    {
                        std::cout << "1. Add quantity" << std::endl;
                        std::cout << "2. Remove quantity" << std::endl;
                        std::cout << "3. Go back" << std::endl;
                        std::cout << std::endl;

                        std::cout << "Enter an option: ";
                        int update_qt_option;
                        update_qt_option = CheckedInputInt("Option must be");
                        //adds quantity
                        if (update_qt_option == 1)
                        {
                            db.UpdateQuantity('a');
                        }
                        //removes quantity
                        else if (update_qt_option == 2)
                        {
                            db.UpdateQuantity('r');
                        }
                        //go back
                        else if (update_qt_option == 3)
                        {
                            break;
                        }
                    }
                }
                //Delete an item
                else if (update_option == 4)
                {
                    db.DeleteItem();
                }
                //go back
                else if (update_option == 5)
                {
                    break;
                }

            }
        }
        //Create a bill and checkout
        else if (option == 4)
        {
            db.Billing();
        }
        //go back
        else if (option == 5)
        {
            break;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////



DbInterface::DbInterface(std::string path_to_file)
{
    //attempts to open the database
    int status_of_open = sqlite3_open(path_to_file.c_str(), &db);

    //if there is an error in opening the database,
    //then set the database pointer to NULL
    if (status_of_open != SQLITE_OK)
    {
        db = NULL;
    }
}

DbInterface::~DbInterface()
{
    //if db points to a database object,
    //then it makes it NULL
    if (db != NULL)
    {
        sqlite3_close(db);    
        db = NULL;
    }
}

bool DbInterface::ExecuteQueryNoResultsBack(sqlite3_stmt* my_statement)
{
    //assume there was a problem executing the query
    bool ret_val = false;
    //execute the query
    int status_of_step = sqlite3_step(my_statement);

    if (status_of_step == SQLITE_DONE)
    {
        ret_val = true;
    }
    //clean up and destroying the statement
    sqlite3_finalize(my_statement);
    
    //returning the success of execution
    return ret_val;
}

bool DbInterface::ExecuteQueryNoResultsBack(std::string sql)
{
    //assume there was a problem executing the query
    bool ret_val = false;
    //create a statement pointer
    sqlite3_stmt* my_statement;
    //create the statement object
    int status_of_prep = sqlite3_prepare_v2(db, sql.c_str(), -1, &my_statement, NULL);

    //check to see if the prepared statement was created
    if (status_of_prep == SQLITE_OK)
    {   
        //execute the query
        ret_val = ExecuteQueryNoResultsBack(my_statement);
    }
    //return the success of the query
    return ret_val;
}

bool DbInterface::PrepareQueryWithResults(std::string sql, sqlite3_stmt*& my_statement)
{
    bool ret_val = false;
    int status_of_prep = sqlite3_prepare_v2(db, sql.c_str(), -1, &my_statement, NULL);

    //check to see if the prepared statement was created
    if(status_of_prep == SQLITE_OK)
    {
        ret_val = true;
    }
    return ret_val;
}

ManagerInterface::ManagerInterface(std::string path_to_file) : DbInterface(path_to_file)
{
    CreateItemsTable();
    //re-indexes the Items table each time the program starts
    std::string sql = "REINDEX Items";
    ExecuteQueryNoResultsBack(sql);
}

void ManagerInterface::CreateItemsTable()
{   
    //query for creating the Items table
    std::string sql = "CREATE TABLE IF NOT EXISTS Items (id INTEGER PRIMARY KEY NOT NULL, itemName TEXT, price REAL, quantity INTEGER, tax REAL, UNIQUE(itemName));"; // UNIQUE to make sure only one entry exists for each item
    //in case there's an error creating the table
    if (!ExecuteQueryNoResultsBack(sql))
    {
        std::cout << "Error creating Items table" << std::endl;
    }
}

void ManagerInterface::InsertItem(std::string itemName, int price, int quantity, double tax)
{   
    std::string sql = "INSERT INTO Items (itemName, price, quantity, tax) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* my_statement;

    //prepares query
    if (PrepareQueryWithResults(sql, my_statement))
    {   
        //binds the given values to the placeholders
        sqlite3_bind_text(my_statement, 1, itemName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(my_statement, 2, price);
        sqlite3_bind_int(my_statement, 3, quantity);
        sqlite3_bind_double(my_statement, 4, tax);

        //executes query
        if(!ExecuteQueryNoResultsBack(my_statement))
        {
            std::cout << "Error Inserting Into Items" << std::endl;
        }
    }
}

void ManagerInterface::DisplayItems()
{
    //
    std::string sql = "Select itemName, price, quantity, tax from Items;";
    sqlite3_stmt* my_statement;
    
    //numbering of the items while printing
    int numeral = 1;
    //get a statement to iterate through
    if (PrepareQueryWithResults(sql, my_statement))
    {
        //get a row from the query
        int status_of_step = sqlite3_step(my_statement);
        //prints the column names
        PrintIndex();
        //while there are more rows
        while (status_of_step == SQLITE_ROW)
        {
            //get item's name
            std::string itemName((char*)sqlite3_column_text(my_statement, 0));
            //get item's price
            int price = sqlite3_column_int(my_statement, 1);
            //get item's quantity
            int quantity = sqlite3_column_int(my_statement, 2);
            //get item's tax
            double tax = sqlite3_column_double(my_statement, 3);
            
            //prints information about the item
            printE(numeral, 6);
            printE(itemName, 18);
            printE(price, 9);
            printE(quantity, 11);
            printE(tax, 7);
            std::cout << std::endl;
            numeral++;
            
            //get the next row
            status_of_step = sqlite3_step(my_statement);
        }
        std::cout << std::endl;
        
        //clean up and delete the compiled statement
        sqlite3_finalize(my_statement);
    }
}

int ManagerInterface::SelectItem()
{
    //selected item
    int rowid = -1;
    //holds all of the row ids from the database
    std::vector<int> rowids;
    
    //query to get all of the items
    std::string sql = "SELECT rowid, itemName, price, quantity, tax FROM Items;";
    sqlite3_stmt* my_statement;

    //get a statement to iterate through
    if(PrepareQueryWithResults(sql, my_statement))
    {
        //get a row from the query
        int status_of_step = sqlite3_step(my_statement);
        int numeral = 1;
        PrintIndex();
        //while there are more rows
        while (status_of_step == SQLITE_ROW)
        {
            rowid = sqlite3_column_int(my_statement, 0);
            rowids.push_back(rowid);

            std::string itemName((char*)sqlite3_column_text(my_statement, 1));
            int price = sqlite3_column_int(my_statement, 2);
            int quantity = sqlite3_column_int(my_statement, 3);
            double tax = sqlite3_column_double(my_statement, 4);

            printE(rowid, 6);
            printE(itemName, 18);
            printE(price, 9);
            printE(quantity, 11);
            printE(tax, 7);
            std::cout << std::endl;
            //get the next row
            status_of_step = sqlite3_step(my_statement);
        }
        std::cout << std::endl;
        sqlite3_finalize(my_statement);
    }
    //select an item
    int option = -1;
    //keeps on taking input from the user until a valid input is given
    while (!(option >= 1 && option <= rowids.size()))
    {
        std::cout << "Select a valid option: ";
        //Checks that the input is an integer
        option = CheckedInputInt("Input must be");
    }
    return option;
}

void ManagerInterface::UpdatePrice()
{
    //returns selected option number
    int selected_option = SelectItem();
    int new_price = -1;
    //keeps on taking input from the user until a valid input is given
    while(new_price <  1)
    {
        std::cout << "Enter the new price: ";
        //Checks that the input is an integer
        new_price = CheckedInputInt("price must be");
    }
    //query for updating price
    std::string sql = "UPDATE Items SET price = ? WHERE rowid = ?;";
    sqlite3_stmt* my_statement;

    //prepares query
    if(PrepareQueryWithResults(sql, my_statement))
    {
        //binds new price and selected option to prepared query
        sqlite3_bind_int(my_statement, 1, new_price);
        sqlite3_bind_int(my_statement, 2, selected_option);
        //executes query
        if (ExecuteQueryNoResultsBack(my_statement))
        {
            std::cout << "Price updated, new price is " << new_price << std::endl << std::endl;
        }
    }
}

//Similar to UpdatePrice
void ManagerInterface::UpdateTax()
{
    int selected_option = SelectItem();
    int new_tax = -1;
    while (new_tax < 0)
    {
        std::cout << "Enter the new tax: ";
        //checks that the input is double
        new_tax = CheckedInputDouble("tax must be");
    }
    std::string sql = "UPDATE Items SET tax = ? WHERE rowid = ?;";
    sqlite3_stmt* my_statement;

    if (PrepareQueryWithResults(sql, my_statement))
    {
        sqlite3_bind_double(my_statement, 1, new_tax);
        sqlite3_bind_int(my_statement, 2, selected_option);

        if (ExecuteQueryNoResultsBack(my_statement))
        {
            std::cout << "Tax updated, new tax is " << new_tax << std::endl << std::endl;
        }
    }   
}

void ManagerInterface::UpdateQuantity(char flag)
{
    //returns selected option number
    int selected_option = SelectItem();

    int current_qt;
    int new_qt;
    int qt_to_change = -1;

    std::string sql_getQt = "SELECT quantity from Items WHERE rowid = ?;";
    sqlite3_stmt* my_statement1;
    //prepares query
    if (PrepareQueryWithResults(sql_getQt, my_statement1))
    {
        sqlite3_bind_int(my_statement1, 1, selected_option);
        //executes query
        int status_of_step = sqlite3_step(my_statement1);
        if(status_of_step == SQLITE_ROW)
        {
            //gets current quantity of selected item
            current_qt = sqlite3_column_int(my_statement1, 0);

            //to add to the inventory
            if (flag == 'a')
            {
                //takes input from the user until a valid input is given
                while (qt_to_change < 0)
                {
                    std::cout << "Enter the quantity to be added: ";
                    qt_to_change = CheckedInputInt("quantity must be");
                }
                //new quantity of the item
                new_qt = current_qt + qt_to_change;
                //query to update the Quantity
                std::string sql_add = "UPDATE Items SET quantity = ? WHERE rowid = ?;";
                sqlite3_stmt* my_statement_add;
                //prepares query
                if(PrepareQueryWithResults(sql_add, my_statement_add))
                {   
                    //binds new quantity
                    sqlite3_bind_int(my_statement_add, 1, new_qt);
                    sqlite3_bind_int(my_statement_add, 2, selected_option);
                    //executes query
                    status_of_step = sqlite3_step(my_statement_add);
                    if (status_of_step == SQLITE_DONE)
                    {
                        std::cout << "Quantity added = " << qt_to_change << ", New quantity = " << new_qt << std::endl << std::endl;
                    }
                    sqlite3_finalize(my_statement_add);
                }
            }
            //similar to add but used to remove items from inventory
            if (flag == 'r')
            {
                //takes input from the user until a valid input is given
                while (qt_to_change < 0 || qt_to_change > current_qt)
                {
                    std::cout << "Enter the quantity to be removed: ";
                    qt_to_change = CheckedInputInt("quantity must be");
                }
                new_qt = current_qt - qt_to_change;

                std::string sql_sub = "UPDATE Items SET quantity = ? WHERE rowid = ?;";
                sqlite3_stmt* my_statement_sub;

                if(PrepareQueryWithResults(sql_sub, my_statement_sub))
                {
                    sqlite3_bind_int(my_statement_sub, 1, new_qt);
                    sqlite3_bind_int(my_statement_sub, 2, selected_option);

                    status_of_step = sqlite3_step(my_statement_sub);
                    if (status_of_step == SQLITE_DONE)
                    {
                        std::cout << "Quantity removed = " << qt_to_change << ", New quantity = " << new_qt << std::endl << std::endl;
                    }
                    sqlite3_finalize(my_statement_sub);
                }    
            }
        }
        sqlite3_finalize(my_statement1);
    }
}   

void ManagerInterface::Billing()
{
    //stores the information of all items in the inventory
    std::vector<int> rowids;
    std::vector<std::string> itemNames;
    std::vector<int> quantity;
    std::vector<int> price;
    std::vector<double> tax;
    
    //stores the rowids of selected items
    std::vector<int> rowids_selected;
    //stores the quantities of selected items
    std::vector<int> quantity_selected;

    //count for total items selected
    int items_selected = 0;

    //query to select all items from the Items table
    std::string sql = "SELECT rowid, itemName, price, quantity, tax FROM Items;";
    sqlite3_stmt* my_statement;
    //prepares query
    if(PrepareQueryWithResults(sql, my_statement))
    {   
        //get a row from the query
        int status_of_step = sqlite3_step(my_statement);
        //while there are more rows
        while (status_of_step == SQLITE_ROW)
        {
            //stores the information of a row
            rowids.push_back(sqlite3_column_int(my_statement, 0));
            itemNames.push_back((char*)sqlite3_column_text(my_statement, 1));
            price.push_back(sqlite3_column_int(my_statement, 2));
            quantity.push_back(sqlite3_column_int(my_statement, 3));
            tax.push_back(sqlite3_column_double(my_statement, 4));

            rowids_selected.push_back(0);
            quantity_selected.push_back(0);

            //get the next row
            status_of_step = sqlite3_step(my_statement);   
        }

        

        while (true)
        {
            //iterates through rowids_selected and updates items_selected
            for (int i : rowids_selected)
            {
                if (i == 1)
                {
                    items_selected++;
                }
            }
            //prompts user to Add an item to bill
            std::cout << "1. Add an item to bill" << std::endl;
            if (items_selected == 0)
            {
                std::cout << std::endl;
            }
            //if one or more items are selected then the user can modify it, see current bill, or go back
            else if (items_selected > 0)
            {   
                std::cout << "2. Remove an item from bill" << std::endl;
                std::cout << "3. See current bill" << std::endl; 
                std::cout << "4. Go back" << std::endl;
            }
            std::cout << "Enter an option: ";

            int option;
            //gets the option from user
            option = CheckedInputInt("Option must be");
            //Adds an item to the bill
            if (option == 1)
            {   
                PrintIndex();
                for (int i : rowids)
                {
                    printE(i, 6);
                    printE(itemNames[i-1], 18);
                    printE(price[i-1], 9);
                    printE(quantity[i-1], 11);
                    printE(tax[i-1], 7);
                    std::cout << std::endl;
                }
                std::cout << std::endl;
                std::cout << "Select an item to be added: ";
                int selected_item;
                int temp_quantity = 0;
                //gets the item to be selected from user
                selected_item = CheckedInputInt("Option must be");
                
                //checks if selected_item is valid
                if ( selected_item > 0 && selected_item <= rowids.size())
                {   
                    //updates the corresponding element in rowids_selected to 1
                    rowids_selected[selected_item-1] = 1;
                    //gets quantity from user until a valid quantity is provided
                    while (temp_quantity < 1 || temp_quantity  > quantity[selected_item-1])
                    {
                        std::cout << "Enter the quantity to be purchased: ";
                        //gets quantity to be purchased from the user
                        temp_quantity = CheckedInputInt("quantity must be");

                        if (temp_quantity < 1)
                        {
                            std::cout << "Please select atleast one item" << std::endl;
                        }
                        else if (temp_quantity  > quantity[selected_item-1])
                        {
                            std::cout << "Not enough " << itemNames[selected_item-1] << " in stock" << std::endl;
                        }
                    }
                    //updates the corresponding element in quantity_selected
                    quantity_selected[selected_item-1] += temp_quantity;
                    //updates the quantity in stock
                    quantity[selected_item-1] -= temp_quantity;
                }
                else
                {
                    std::cout << "Invalid option" << std::endl << std::endl;
                }
                
            }
            //Removes an item from the bill
            else if (option == 2 && items_selected > 0)
            {
               PrintIndex();
               std::cout << std::endl;
               //prints the current inventory
               for (int i : rowids)
                {   
                    if(rowids_selected[i-1] == 1)
                    {
                        printE(i, 6);
                        printE(itemNames[i-1], 18);
                        printE(price[i-1], 9);
                        printE(quantity[i-1], 11);
                        printE(tax[i-1], 7);
                        std::cout << std::endl;
                    }
                }
                std::cout << "Select an item to be removed: ";
                //gets the selected item
                int selected_item = CheckedInputInt("Option must be");
                //checks if the selected item is valid
                if (selected_item > 0 && selected_item <= rowids.size())
                {    
                    if (rowids_selected[selected_item-1] == 0)
                    {
                        std::cout << "Item " << itemNames[selected_item-1] << " is not is current bill" << std::endl;
                    }
                    else
                    {
                        //updates the inventory
                        rowids_selected[selected_item-1] = 0;
                        quantity[selected_item-1] += quantity_selected[selected_item-1];
                        quantity_selected[selected_item-1] = 0;
                    }
                }
                else 
                {
                    std::cout << "Invalid option" << std::endl << std::endl;
                }
            }
            //See current bill
            else if (option == 3 && items_selected > 0)
            {   
                double total_bill = 0;
                std::cout << std::endl;
                std::cout << "Current Items are: " << std::endl;
                PrintIndex();
                //print current items in bill
                for (int i : rowids)
                {
                    if (rowids_selected[i-1] == 1)
                    {
                        printE(i, 6);
                        printE(itemNames[i-1], 18);
                        printE(price[i-1], 9);
                        printE(quantity_selected[i-1], 11);
                        printE(tax[i-1], 7);
                        std::cout << std::endl;
                        total_bill += (price[i-1] + price[i-1] * tax[i-1] / 100) * quantity_selected[i-1];
                    }
                }
                std::cout << "Total Amount: " << total_bill << std::endl;
                if ((int)total_bill < total_bill)
                    std::cout << "Rounded off Amount: " << (int)total_bill + 1 << std::endl << std::endl;
                else
                    std::cout << "Rounded off Amount: " << (int)total_bill<< std::endl << std::endl;
                std::cout << "1. Checkout" << std::endl;
                std::cout << "2. Go back" << std::endl;
                std::cout << std::endl;

                std::cout << "Enter an option: ";
                //gets option from the user
                int checkout_option = CheckedInputInt("option must be: ");

                while (true)
                {
                    if (checkout_option == 1)
                    {
                        //if the user checks out, then updates the database
                        std::string sql_checkout = "UPDATE Items SET quantity = ? WHERE rowid = ?;";
                        for (int i : rowids)
                        {
                            if (rowids_selected[i-1] == 1)
                            {
                                sqlite3_stmt* my_statement_checkout;
                                if (PrepareQueryWithResults(sql_checkout, my_statement_checkout))
                                {
                                    sqlite3_bind_int(my_statement_checkout, 1, quantity[i-1]);
                                    sqlite3_bind_int(my_statement_checkout, 2, i);

                                    ExecuteQueryNoResultsBack(my_statement_checkout);
                                }
                            }
                        }
                        std::cout << "Checkout Successfull. Database Updated." << std::endl << std::endl;
                        break;
                    }
                    //breaks out of the loop if user decides to go back
                    else if (checkout_option == 2)
                    {
                        break;
                    }
                }
            }
            //breaks out of the loop if user decides to go back
            else if (option == 4)
            {
                break;
            }  
        }
        //cleans up and destroy the compiled statement
        sqlite3_finalize(my_statement);
    }
}

void ManagerInterface::PrintIndex()
{
    //prints the column names
    printE("Id.", 6);
    printE("Item Name", 18);
    printE("Price", 9);
    printE("Quantity", 11);
    printE("Tax", 7);
    std::cout << std::endl;
}

void ManagerInterface::DeleteItem()
{
    //returns the selected option
    int selected_option = SelectItem();
    //query to delete an item
    std::string sql = "DELETE FROM Items WHERE rowid = ?;";
    sqlite3_stmt* my_statement;
    //prepares query
    if (PrepareQueryWithResults(sql, my_statement))
    {
        //binds item to be deleted
        sqlite3_bind_int(my_statement, 1, selected_option);
        //executes query
        if (ExecuteQueryNoResultsBack(my_statement))
        {
            std::cout << "Item Deleted Successfully" << std::endl << std::endl;
            std::string sql_reindex = "REINDEX Items;";
            //re-indexes the Items table
            ExecuteQueryNoResultsBack(sql_reindex);
        }
        else
        {
            std::cout << "Failed to delete item" << std::endl << std::endl;
        }  
    }
}





//Capitalizes a given string
void Capitalize(std::string& str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (std::isalpha(str[i]) && std::islower(str[i]))
        {
            str[i] = (char)(int(str[i])-32);
        }
        i++;
    }
}

//takes input from the user until an int is given and returns it
int CheckedInputInt(std::string err_msg)
{
    int i;
    std::cin >> i;   
    while (true)
    {
        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout<< err_msg << " an Integer" << std::endl;
            std::cin>> i;
        }
        if(!std::cin.fail())
        {
            break;
        }
    }
    return i;
}
//takes input from the user until a double is given and returns it
double CheckedInputDouble(std::string err_msg)
{
    double d;
    std::cin >> d;
    while (true)
    {
        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout<< err_msg << " Real" << std::endl;
            std::cin>> d;
        }
        if(!std::cin.fail())
        {
            break;
        }
    }
    return d;
}

//to format the output
template<typename T> void printE(T t, int width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}