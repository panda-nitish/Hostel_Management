#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- Structures ---------------- */

struct Student {
    int id;
    char name[50];
    char gender[10];
    char hostel_name[30];
    int room_no;
    char room_type[20];
    char course[50];
    char guardian_name[50];
    char guardian_contact[15];
    float fees;           // total hostel fee (calculated)
    char password[20];    // student password
};

struct Room {
    int room_no;
    char type[20];    // Three_Sharing / Four_Sharing / Six_Sharing / Eight_Sharing
    int capacity;
    int occupied;
    float rent;
    char status[20];
};

struct Fee {
    int student_id;
    float amount;
    char date[15];
    char mode[20]; // Cash / UPI / Card
};

struct Admin {
    char username[20];
    char password[20];
} ;

/* ---------------- Global Variables ---------------- */

struct Student students[200];
struct Room rooms[100];
struct Fee fees[500];
struct Admin admin = {"panda_nitish963", "151947"}; // default; overwritten by load if file exists

int count = 0;       // student count
int room_count = 0;  // room count
int fee_count = 0;   // fee count

/* ---------------- Function Declarations ---------------- */

void addStudent();              // admin adds student (includes password)
void viewStudents();
void searchStudent();
void updateStudent();
void deleteStudent();
void saveToFile();
void loadFromFile();

void addRoom();
void viewByHostel();
void viewRooms();
void searchRoom();
void updateRoom();
void saveRooms();
void loadRooms();

void payFee();
void viewFees();
void saveFees();
void loadFees();
void viewPendingFees();

int adminLogin();
int studentLogin();

void saveAdmin();
void loadAdmin();
void changeAdminPassword();
void changeAdminUsername();

void changeStudentPassword(int studentIndex);
void registerStudent();         // open registration (self-registration)

/* ---------------- Fee helper ---------------- */

/*
 * recordInitialPayment(student_id)
 * Prompts for amount paid now and payment mode and optionally date,
 * then appends a fees[] entry and increments fee_count.
 */
void recordInitialPayment(int student_id) {
    float paidNow = 0.0f;
    char mode[20] = "Cash";
    char date[16] = "At_Registration";

    printf("Enter amount paid now (enter 0 for none): ");
    scanf("%f", &paidNow);
    if (paidNow > 0.0f) {
        printf("Enter payment mode (Cash/UPI/Card): ");
        scanf("%s", mode);
        // optional: allow entering a date, otherwise use "At_Registration"
        printf("Enter payment date (dd-mm-yyyy) or 0 to use At_Registration: ");
        scanf("%s", date);
        if (strcmp(date, "0") == 0) strcpy(date, "At_Registration");

        fees[fee_count].student_id = student_id;
        fees[fee_count].amount = paidNow;
        strncpy(fees[fee_count].mode, mode, sizeof(fees[fee_count].mode)-1);
        fees[fee_count].mode[sizeof(fees[fee_count].mode)-1] = '\0';
        strncpy(fees[fee_count].date, date, sizeof(fees[fee_count].date)-1);
        fees[fee_count].date[sizeof(fees[fee_count].date)-1] = '\0';
        fee_count++;

        // auto-save fees immediately for persistence
        saveFees();
        saveToFile(); // also save students file to keep data consistent if needed

        printf("Initial payment of ₹%.2f recorded successfully (Mode: %s, Date: %s).\n", paidNow, mode, date);
    } else {
        printf("No payment recorded now.\n");
    }
}

/* ---------------- Student Functions ---------------- */

void addStudent() {
    printf("\nEnter Student ID: ");
    scanf("%d", &students[count].id);
    printf("Enter Name: ");
    scanf(" %[^\n]", students[count].name);
    printf("Enter Gender: ");
    scanf("%s", students[count].gender);

    printf("Select Hostel:\n");
    printf("1. GANGA - A\n");
    printf("2. GANGA - B\n");
    printf("3. VEDAVATI\n");
    printf("4. NEW HOSTEL\n");
    printf("Enter choice: ");
    int hch;
    scanf("%d", &hch);

    switch(hch) {
        case 1: strcpy(students[count].hostel_name, "GANGA - A"); break;
        case 2: strcpy(students[count].hostel_name, "GANGA - B"); break;
        case 3: strcpy(students[count].hostel_name, "VEDAVATI"); break;
        case 4: strcpy(students[count].hostel_name, "NEW HOSTEL"); break;
        default: strcpy(students[count].hostel_name, "Unassigned"); break;
    }

    printf("Enter Room No: ");
    scanf("%d", &students[count].room_no);

    printf("Select Room Type:\n");
    printf("1. Three_Sharing\n2. Four_Sharing\n3. Six_Sharing\n4. Eight_Sharing\n");
    printf("Enter choice: ");
    int rch;
    scanf("%d", &rch);

    switch(rch) {
        case 1: strcpy(students[count].room_type, "Three_Sharing"); break;
        case 2: strcpy(students[count].room_type, "Four_Sharing"); break;
        case 3: strcpy(students[count].room_type, "Six_Sharing"); break;
        case 4: strcpy(students[count].room_type, "Eight_Sharing"); break;
        default: strcpy(students[count].room_type, "Unknown"); break;
    }

    printf("Enter Course: ");
    scanf(" %[^\n]", students[count].course);
    printf("Enter Guardian Name: ");
    scanf(" %[^\n]", students[count].guardian_name);
    printf("Enter Guardian Contact: ");
    scanf("%s", students[count].guardian_contact);

    // -------- Fee Calculation --------
    float baseFee = 0.0f;

    // Hostel-wise base fee (per-month or per-term as your policy)
    if (strcmp(students[count].hostel_name, "GANGA - A") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "GANGA - B") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "VEDAVATI") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "NEW HOSTEL") == 0) baseFee = 100000.0f;
    else baseFee = 5000.0f; // fallback

    // Room type multiplier (fewer sharing -> higher cost)
    if (strcmp(students[count].room_type, "Three_Sharing") == 0) baseFee *= 2.10f;  // more expensive
    else if (strcmp(students[count].room_type, "Four_Sharing") == 0) baseFee *= 1.35f;
    else if (strcmp(students[count].room_type, "Six_Sharing") == 0) baseFee *= 1.10f;
    else if (strcmp(students[count].room_type, "Eight_Sharing") == 0) baseFee *= 0.90f;
    else baseFee *= 1.0f; // unknown -> no multiplier

    students[count].fees = baseFee;

    printf("Automatically calculated total hostel fee: ₹%.2f\n", students[count].fees);

    // ask password
    printf("Set Password for Student (max 19 chars): ");
    scanf("%s", students[count].password);

    // Record initial payment at registration (admin adding student)
    recordInitialPayment(students[count].id);

    count++;
    printf("Student added successfully!\n");
}

void registerStudent() {
    // Self registration: same fields as addStudent but keep registration flow
    printf("\n--- Student Registration ---\n");

    printf("Enter Student ID: ");
    scanf("%d", &students[count].id);
    printf("Enter Name: ");
    scanf(" %[^\n]", students[count].name);
    printf("Enter Gender: ");
    scanf("%s", students[count].gender);

    printf("Select Hostel:\n");
    printf("1. GANGA - A\n");
    printf("2. GANGA - B\n");
    printf("3. VEDAVATI\n");
    printf("4. NEW HOSTEL\n");
    printf("Enter choice: ");
    int hch;
    scanf("%d", &hch);

    switch(hch) {
        case 1: strcpy(students[count].hostel_name, "GANGA - A"); break;
        case 2: strcpy(students[count].hostel_name, "GANGA - B"); break;
        case 3: strcpy(students[count].hostel_name, "VEDAVATI"); break;
        case 4: strcpy(students[count].hostel_name, "NEW HOSTEL"); break;
        default: strcpy(students[count].hostel_name, "Unassigned"); break;
    }

    printf("Enter Room No: ");
    scanf("%d", &students[count].room_no);

    printf("Select Room Type:\n");
    printf("1. Three_Sharing\n2. Four_Sharing\n3. Six_Sharing\n4. Eight_Sharing\n");
    printf("Enter choice: ");
    int rch;
    scanf("%d", &rch);

    switch(rch) {
        case 1: strcpy(students[count].room_type, "Three_Sharing"); break;
        case 2: strcpy(students[count].room_type, "Four_Sharing"); break;
        case 3: strcpy(students[count].room_type, "Six_Sharing"); break;
        case 4: strcpy(students[count].room_type, "Eight_Sharing"); break;
        default: strcpy(students[count].room_type, "Unknown"); break;
    }

    printf("Enter Course: ");
    scanf(" %[^\n]", students[count].course);
    printf("Enter Guardian Name: ");
    scanf(" %[^\n]", students[count].guardian_name);
    printf("Enter Guardian Contact: ");
    scanf("%s", students[count].guardian_contact);

    // Fee calculation (same auto logic)
    float baseFee = 0.0f;
    if (strcmp(students[count].hostel_name, "GANGA - A") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "GANGA - B") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "VEDAVATI") == 0) baseFee = 100000.0f;
    else if (strcmp(students[count].hostel_name, "NEW HOSTEL") == 0) baseFee = 100000.0f;
    else baseFee = 5000.0f;

    if (strcmp(students[count].room_type, "Three_Sharing") == 0) baseFee *= 2.10f;
    else if (strcmp(students[count].room_type, "Four_Sharing") == 0) baseFee *= 1.35f;
    else if (strcmp(students[count].room_type, "Six_Sharing") == 0) baseFee *= 1.10f;
    else if (strcmp(students[count].room_type, "Eight_Sharing") == 0) baseFee *= 0.90f;
    students[count].fees = baseFee;

    printf("Automatically calculated total hostel fee: ₹%.2f\n", students[count].fees);

    printf("Set Password for Student (max 19 chars): ");
    scanf("%s", students[count].password);

    // Record initial payment
    recordInitialPayment(students[count].id);

    // After registration, save to disk immediately
    saveToFile();
    printf("Registration complete. You can now login with your ID and password.\n");

    count++;
}

/* ---------------- View / Search / Update / Delete Students ---------------- */

void viewStudents() {
    if(count == 0) {
        printf("\nNo students registered.\n");
        return;
    }

    printf("\n%-8s %-20s %-10s %-15s %-8s %-15s %-15s %-10s %-10s %-10s\n",
           "ID", "Name", "Gender", "Hostel", "Room", "RoomType", "Course", "TotalFee", "Paid", "Balance");
    printf("----------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        float paid = 0.0f;
        for (int j = 0; j < fee_count; j++) {
            if (fees[j].student_id == students[i].id)
                paid += fees[j].amount;
        }
        float balance = students[i].fees - paid;
        if (balance < 0.0f) balance = 0.0f;

        printf("%-8d %-20s %-10s %-15s %-8d %-15s %-15s %-10.2f %-10.2f %-10.2f\n",
               students[i].id, students[i].name, students[i].gender,
               students[i].hostel_name, students[i].room_no, students[i].room_type,
               students[i].course, students[i].fees, paid, balance);
    }
}

void searchStudent() {
    int id, found = 0;
    printf("\nEnter Student ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            printf("\nStudent Found:\n");
            printf("ID: %d\nName: %s\nGender: %s\nHostel: %s\nRoom: %d\nRoom Type: %s\nCourse: %s\nGuardian: %s\nContact: %s\nFees: %.2f\n",
                   students[i].id, students[i].name, students[i].gender, students[i].hostel_name,
                   students[i].room_no, students[i].room_type, students[i].course,
                   students[i].guardian_name, students[i].guardian_contact, students[i].fees);

            // show payments for this student
            float totalPaid = 0.0f;
            printf("\n--- Payments ---\n");
            for (int j = 0; j < fee_count; j++) {
                if (fees[j].student_id == students[i].id) {
                    printf("Date: %s | Amount: %.2f | Mode: %s\n", fees[j].date, fees[j].amount, fees[j].mode);
                    totalPaid += fees[j].amount;
                }
            }
            printf("Total Paid: %.2f | Balance: %.2f\n", totalPaid, (students[i].fees - totalPaid) > 0 ? (students[i].fees - totalPaid) : 0.0f);

            found = 1;
            break;
        }
    }

    if (!found) printf("Student not found!\n");
}

void viewByHostel() {
    int ch;
    printf("\nSelect Hostel to View:\n");
    printf("1. GANGA - A\n2. GANGA - B\n3. VEDAVATI\n4. NEW HOSTEL\n");
    printf("Enter choice: ");
    scanf("%d", &ch);

    char hostel[30];
    switch(ch) {
        case 1: strcpy(hostel, "GANGA - A"); break;
        case 2: strcpy(hostel, "GANGA - B"); break;
        case 3: strcpy(hostel, "VEDAVATI"); break;
        case 4: strcpy(hostel, "NEW HOSTEL"); break;
        default: printf("Invalid choice.\n"); return;
    }

    printf("\n=== Students in %s ===\n", hostel);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].hostel_name, hostel) == 0) {
            printf("ID: %d | Name: %s | Room: %d | RoomType: %s | Course: %s | Fees: %.2f\n",
                   students[i].id, students[i].name, students[i].room_no, students[i].room_type,
                   students[i].course, students[i].fees);
            found = 1;
        }
    }
    if (!found) printf("No students found in this hostel.\n");
}

void updateStudent() {
    int id, found = 0;
    printf("\nEnter Student ID to update: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            printf("Enter new Room No: ");
            scanf("%d", &students[i].room_no);
            printf("Enter new Fees (enter 0 to auto-recalculate based on hostel+room type): ");
            float newfees;
            scanf("%f", &newfees);
            if (newfees > 0.0f) {
                students[i].fees = newfees;
            } else {
                // recalc using same logic as addStudent
                float baseFee = 0.0f;
                if (strcmp(students[i].hostel_name, "GANGA - A") == 0) baseFee = 5000.0f;
                else if (strcmp(students[i].hostel_name, "GANGA - B") == 0) baseFee = 4800.0f;
                else if (strcmp(students[i].hostel_name, "VEDAVATI") == 0) baseFee = 5200.0f;
                else if (strcmp(students[i].hostel_name, "NEW HOSTEL") == 0) baseFee = 6000.0f;
                if (strcmp(students[i].room_type, "Three_Sharing") == 0) baseFee *= 1.20f;
                else if (strcmp(students[i].room_type, "Four_Sharing") == 0) baseFee *= 1.00f;
                else if (strcmp(students[i].room_type, "Six_Sharing") == 0) baseFee *= 0.80f;
                else if (strcmp(students[i].room_type, "Eight_Sharing") == 0) baseFee *= 0.60f;
                students[i].fees = baseFee;
            }
            printf("Record updated!\n");
            found = 1;
            break;
        }
    }

    if (!found) printf("Student not found!\n");
}

void deleteStudent() {
    int id, found = 0;
    printf("\nEnter Student ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            for (int j = i; j < count - 1; j++) {
                students[j] = students[j + 1];
            }
            count--;
            printf("Student deleted successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) printf("Student not found!\n");
}

/* ---------------- Room Functions ---------------- */

void addRoom() {
    printf("\nEnter Room Number: ");
    scanf("%d", &rooms[room_count].room_no);
    printf("Enter Room Type (Three_Sharing/Four_Sharing/Six_Sharing/Eight_Sharing): ");
    scanf("%s", rooms[room_count].type);
    printf("Enter Capacity: ");
    scanf("%d", &rooms[room_count].capacity);
    printf("Enter Rent: ");
    scanf("%f", &rooms[room_count].rent);
    printf("Enter Status (Available/Full/Maintenance): ");
    scanf("%s", rooms[room_count].status);
    rooms[room_count].occupied = 0;
    room_count++;
    printf("Room added successfully!\n");
}

void viewRooms() {
    if(room_count == 0) { printf("\nNo rooms found.\n"); return; }
    printf("\n%-8s %-18s %-10s %-10s %-10s %-15s\n",
           "RoomNo", "Type", "Capacity", "Occupied", "Rent", "Status");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < room_count; i++) {
        printf("%-8d %-18s %-10d %-10d %-10.2f %-15s\n",
               rooms[i].room_no, rooms[i].type, rooms[i].capacity,
               rooms[i].occupied, rooms[i].rent, rooms[i].status);
    }
}

void searchRoom() {
    int num, found = 0;
    printf("\nEnter Room Number to search: ");
    scanf("%d", &num);

    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_no == num) {
            printf("Room %d found.\nType: %s\nCapacity: %d\nRent: %.2f\nStatus: %s\n",
                   rooms[i].room_no, rooms[i].type, rooms[i].capacity,
                   rooms[i].rent, rooms[i].status);
            found = 1;
            break;
        }
    }

    if (!found) printf("Room not found!\n");
}

void updateRoom() {
    int num, found = 0;
    printf("\nEnter Room Number to update: ");
    scanf("%d", &num);

    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_no == num) {
            printf("Enter new Rent: ");
            scanf("%f", &rooms[i].rent);
            printf("Enter new Status: ");
            scanf("%s", rooms[i].status);
            printf("Room updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) printf("Room not found!\n");
}

/* ---------------- Fee Functions ---------------- */

void payFee() {
    printf("\nEnter Student ID: ");
    scanf("%d", &fees[fee_count].student_id);
    printf("Enter Amount: ");
    scanf("%f", &fees[fee_count].amount);
    printf("Enter Payment Date (dd-mm-yyyy): ");
    scanf("%s", fees[fee_count].date);
    printf("Enter Payment Mode (Cash/UPI/Card): ");
    scanf("%s", fees[fee_count].mode);

    fee_count++;

    // auto-save payments to disk immediately
    saveFees();
    saveToFile();
    printf("Fee payment recorded successfully!\n");
}

void viewPendingFees() {
    if (count == 0) {
        printf("\nNo students registered.\n");
        return;
    }

    int any = 0;
    printf("\n=== STUDENTS WITH PENDING FEES ===\n");
    printf("%-8s %-20s %-15s %-10s %-10s %-10s\n",
           "ID", "Name", "Hostel", "TotalFee", "Paid", "Balance");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        float paid = 0;
        for (int j = 0; j < fee_count; j++) {
            if (fees[j].student_id == students[i].id)
                paid += fees[j].amount;
        }
        float balance = students[i].fees - paid;
        if (balance > 0.01f) {  // avoid float precision issue
            printf("%-8d %-20s %-15s %-10.2f %-10.2f %-10.2f\n",
                   students[i].id, students[i].name, students[i].hostel_name,
                   students[i].fees, paid, balance);
            any = 1;
        }
    }

    if (!any)
        printf("All students have cleared their fees!\n");
}

void viewFees() {
    if(fee_count == 0) { printf("\nNo fee records.\n"); return; }
    printf("\n%-10s %-10s %-15s %-10s\n",
           "StudentID", "Amount", "Date", "Mode");
    printf("---------------------------------------------\n");
    for (int i = 0; i < fee_count; i++) {
        printf("%-10d %-10.2f %-15s %-10s\n",
               fees[i].student_id, fees[i].amount,
               fees[i].date, fees[i].mode);
    }
}

/* ---------------- File Handling ---------------- */

void saveToFile() {
    FILE *fp = fopen("hostel.dat", "wb");
    if (!fp) { printf("Error saving hostel.dat\n"); return; }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(struct Student), count, fp);
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen("hostel.dat", "rb");
    if (fp == NULL) return;
    fread(&count, sizeof(int), 1, fp);
    fread(students, sizeof(struct Student), count, fp);
    fclose(fp);
}

void saveRooms() {
    FILE *fp = fopen("rooms.dat", "wb");
    if (!fp) { printf("Error saving rooms.dat\n"); return; }
    fwrite(&room_count, sizeof(int), 1, fp);
    fwrite(rooms, sizeof(struct Room), room_count, fp);
    fclose(fp);
}

void loadRooms() {
    FILE *fp = fopen("rooms.dat", "rb");
    if (fp == NULL) return;
    fread(&room_count, sizeof(int), 1, fp);
    fread(rooms, sizeof(struct Room), room_count, fp);
    fclose(fp);
}

void saveFees() {
    FILE *fp = fopen("fees.dat", "wb");
    if (!fp) { printf("Error saving fees.dat\n"); return; }
    fwrite(&fee_count, sizeof(int), 1, fp);
    fwrite(fees, sizeof(struct Fee), fee_count, fp);
    fclose(fp);
}

void loadFees() {
    FILE *fp = fopen("fees.dat", "rb");
    if (fp == NULL) return;
    fread(&fee_count, sizeof(int), 1, fp);
    fread(fees, sizeof(struct Fee), fee_count, fp);
    fclose(fp);
}

void saveAdmin() {
    FILE *fp = fopen("admin.dat", "wb");
    if (!fp) { printf("Error saving admin.dat\n"); return; }
    fwrite(&admin, sizeof(struct Admin), 1, fp);
    fclose(fp);
}

void loadAdmin() {
    FILE *fp = fopen("admin.dat", "rb");
    if (fp == NULL) return;
    fread(&admin, sizeof(struct Admin), 1, fp);
    fclose(fp);
}

/* ---------------- Password / Login Utilities ---------------- */

void changeAdminPassword() {
    char oldp[20], newp[20], confirm[20];
    printf("\nEnter current admin password: ");
    scanf("%s", oldp);
    if (strcmp(oldp, admin.password) != 0) {
        printf("Incorrect password.\n");
        return;
    }
    printf("Enter new password: ");
    scanf("%s", newp);
    printf("Confirm new password: ");
    scanf("%s", confirm);
    if (strcmp(newp, confirm) != 0) {
        printf("Passwords do not match.\n");
        return;
    }
    strcpy(admin.password, newp);
    saveAdmin();
    printf("Admin password changed successfully.\n");
}

void changeAdminUsername() {
    char oldu[20], newu[20], confirm[20];
    printf("\nEnter current admin username: ");
    scanf("%s", oldu);

    if (strcmp(oldu, admin.username) != 0) {
        printf("Incorrect username.\n");
        return;
    }

    printf("Enter new username: ");
    scanf("%s", newu);
    printf("Confirm new username: ");
    scanf("%s", confirm);

    if (strcmp(newu, confirm) != 0) {
        printf("Usernames do not match.\n");
        return;
    }

    strcpy(admin.username, newu);
    saveAdmin();
    printf("Admin username changed successfully.\n");
}

void changeStudentPassword(int studentIndex) {
    char oldp[20], newp[20], confirm[20];
    printf("\nEnter current password: ");
    scanf("%s", oldp);
    if (strcmp(oldp, students[studentIndex].password) != 0) {
        printf("Incorrect password.\n");
        return;
    }
    printf("Enter new password: ");
    scanf("%s", newp);
    printf("Confirm new password: ");
    scanf("%s", confirm);
    if (strcmp(newp, confirm) != 0) {
        printf("Passwords do not match.\n");
        return;
    }
    strcpy(students[studentIndex].password, newp);
    saveToFile();
    printf("Password changed successfully.\n");
}

/* ---------------- Login System ---------------- */

int adminLogin() {
    char uname[20], pass[20];
    printf("\n=== ADMIN LOGIN ===\n");
    printf("Username: ");
    scanf("%s", uname);
    printf("Password: ");
    scanf("%s", pass);

    if (strcmp(uname, admin.username) == 0 && strcmp(pass, admin.password) == 0) {
        printf("\nLogin successful! Welcome, Admin.\n");
        return 1;
    } else {
        printf("\nInvalid username or password!\n");
        return 0;
    }
}

int studentLogin() {
    int id, found = -1;
    char pass[20];
    printf("\n=== STUDENT LOGIN ===\n");
    printf("Enter your Student ID: ");
    scanf("%d", &id);
    printf("Enter your password: ");
    scanf("%s", pass);

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            if (strcmp(students[i].password, pass) == 0) {
                found = i;
            }
            break;
        }
    }

    if (found == -1) {
        printf("Invalid ID or password. Please contact admin if you are not registered.\n");
        return 0;
    }

    printf("\nWelcome, %s!\n", students[found].name);
    printf("Room No: %d | Course: %s | Fees: %.2f\n",
           students[found].room_no, students[found].course, students[found].fees);

    int ch;
    while (1) {
        printf("\n--- Student Menu ---\n");
        printf("1. View My Details\n2. View My Fee Payments\n3. Change Password\n4. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &ch);

        if (ch == 1) {
            printf("\nStudent ID: %d\nName: %s\nGender: %s\nRoom: %d\nRoom Type: %s\nCourse: %s\nGuardian: %s\nContact: %s\nFees: %.2f\n",
                   students[found].id, students[found].name, students[found].gender,
                   students[found].room_no, students[found].room_type, students[found].course, students[found].guardian_name,
                   students[found].guardian_contact, students[found].fees);
        }
        else if (ch == 2) {
            printf("\n--- My Fee Payments ---\n");
            float totalPaid = 0;
            int any = 0;

            for (int j = 0; j < fee_count; j++) {
                if (fees[j].student_id == students[found].id) {
                    printf("Date: %s | Amount: %.2f | Mode: %s\n",
                           fees[j].date, fees[j].amount, fees[j].mode);
                    totalPaid += fees[j].amount;
                    any = 1;
                }
            }

            if (!any)
                printf("No fee records found for you.\n");

            float balance = students[found].fees - totalPaid;
            if (balance < 0) balance = 0;

            printf("\nTotal Fee: %.2f\nTotal Paid: %.2f\nRemaining Balance: %.2f\n",
                   students[found].fees, totalPaid, balance);
        }
        else if (ch == 3) {
            changeStudentPassword(found);
        } else if (ch == 4) {
            printf("Logging out...\n");
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }

    return 1;
}

/* ---------------- MAIN FUNCTION ---------------- */

int main() {
    int choice;
    // load saved data
    loadAdmin();
    loadFromFile();
    loadRooms();
    loadFees();

    while (1) {
        printf("\n===== HOSTEL MANAGEMENT SYSTEM =====\n");
        printf("1. Admin Login\n");
        printf("2. Student Login\n");
        printf("3. Student Registration\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            if (!adminLogin()) continue;

            // Admin menu
            while (1) {
                printf("\n===== ADMIN MENU =====\n");
                printf("1. Add Student (register)\n");
                printf("2. View All Students\n");
                printf("3. Search Student\n");
                printf("4. Update Student\n");
                printf("5. Delete Student\n");
                printf("6. Room Management\n");
                printf("7. Fee Management\n");
                printf("8. Change Admin Password\n");
                printf("9. Change Admin Username\n");
                printf("10. View Students by Hostel\n");
                printf("11. View Fee Balances (Pending Dues)\n");
                printf("12. Save & Logout\n");
                printf("Enter your choice: ");
                scanf("%d", &choice);

                if (choice == 1) {
                    addStudent();
                } else if (choice == 2) {
                    viewStudents();
                } else if (choice == 3) {
                    searchStudent();
                } else if (choice == 4) {
                    updateStudent();
                } else if (choice == 5) {
                    deleteStudent();
                } else if (choice == 6) {
                    printf("\n--- Room Management ---\n");
                    printf("1. Add Room\n2. View Rooms\n3. Search Room\n4. Update Room\n");
                    printf("Enter choice: "); int rc; scanf("%d", &rc);
                    if(rc==1) addRoom();
                    else if(rc==2) viewRooms();
                    else if(rc==3) searchRoom();
                    else if(rc==4) updateRoom();
                    else printf("Invalid!\n");
                } else if (choice == 7) {
                    printf("\n--- Fee Management ---\n");
                    printf("1. Pay Fee\n2. View Fees\n");
                    printf("Enter choice: "); int fc; scanf("%d", &fc);
                    if(fc==1) payFee();
                    else if(fc==2) viewFees();
                    else printf("Invalid!\n");
                } else if (choice == 8) {
                    changeAdminPassword();
                } else if (choice == 9) {
                    changeAdminUsername();
                } else if (choice == 12) {
                    saveToFile();
                    saveRooms();
                    saveFees();
                    saveAdmin();
                    printf("All data saved. Logging out...\n");
                    break;
                }
                else if (choice == 11) {
                    viewPendingFees();
                }
                else if (choice == 10) {
                    viewByHostel();
                }else {
                    printf("Invalid choice!\n");
                }
                // auto-save after certain admin actions to be safer
                saveToFile();
                saveRooms();
                saveFees();
                saveAdmin();
            }
        } else if (choice == 2) {
            studentLogin();
        } else if (choice == 3) {
            registerStudent();
        } else if (choice == 4) {
            printf("Exiting...\n");
            saveToFile();
            saveRooms();
            saveFees();
            saveAdmin();
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
