#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_STACK_SIZE 100
#define MAX_HASH_SIZE 10
#define MAX_STRING 50

//  ADT
typedef struct Tiket {
    char destinasi[MAX_STRING];
    char seat[5];
    char time[MAX_STRING];
} Tiket;
typedef struct Penumpang {
    Tiket tiket;
    char nama[MAX_STRING];
    char passport[MAX_STRING];
    char nomorTiket[MAX_STRING];
    int ID;
} Penumpang;

// QUEUE
typedef struct Node {
    Penumpang data;
    struct Node* next;
} Node;
typedef struct Queue {
    Node* head;
    Node* tail;
} Queue;

// STACK
typedef struct Stack {
    Penumpang data[MAX_STACK_SIZE];
    int top;
} Stack;

//  TREE
typedef struct BSTNode {
    int ID;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

// HASH
typedef struct HashNode {
    Penumpang data;
    struct HashNode* next;
} HashNode;
typedef struct HashTable {
    HashNode* table[MAX_HASH_SIZE];
} HashTable;

// ARRAY CONST
const char* listDestinasi[] = {"Jakarta", "Bali", "Malang", "Lombok", "Solo", "Bandung"};
const char* listCode[] = {"01", "02", "03", "04", "05", "06"};
const char* waktu[] = {"2023-06-27 08:00", "2023-06-27 10:00", "2023-06-27 12:00", "2023-06-27 14:00", "2023-06-27 16:00", "2023-06-27 18:00"};

//  VAR GLOBAL
Queue antrian;
Stack deletedStack; 
BSTNode* bstRoot = NULL;
HashTable hashTable;
int penumpangID = 100;

//  QUEUE
// Inisialisasi Queue --> Saat Program dimulai
void initQueue(Queue* q) {
    q->head = q->tail = NULL;
}
// Memeriksa apakah queue kosong --> Saat Pesan/Hapus/Cari/Perbarui/Sorting Tiket
int isEmptyQueue(Queue* q) {
    return q->head == NULL;
}
// Menambahkan elemen ke dalam queue --> Saat Pesan/Restore Tiket
void enqueue(Queue* q, Penumpang p) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = p;
    temp->next = NULL;
    if (isEmptyQueue(q)) {
        q->head = q->tail = temp;
    } else {
        q->tail->next = temp;
        q->tail = temp;
    }
    printf("Debug: Penumpang dengan ID [%d] telah ditambahkan ke antrian.\n", p.ID);  // Tambahkan ini untuk debugging
}

//  STACK
// Inisialisasi Stack --> Saat Program Dimulai
void initStack(Stack* s) {
    s->top = -1;
}
// Memeriksa apakah stack kosong --> Saat Restore Tiket
int isEmptyStack(Stack* s) {
    return s->top == -1;
}
// Push elemen ke stack --> Saat Hapus Tiket
void push(Stack* s, Penumpang p) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->data[++s->top] = p;
    } else {
        printf("Stack penuh, tidak dapat menambahkan data penumpang.\n");
    }
}
// Pop elemen dari stack --> Saat Restore Tiket
Penumpang pop(Stack* s) {
    if (isEmptyStack(s)) {
        printf("Stack kosong, tidak ada data penumpang yang bisa dihapus.\n");
        exit(1);
    } else {
        return s->data[s->top--];
    }
}

// TREE
// Fungsi untuk membuat node Tree baru --> Saat Check-in 
BSTNode* createBSTNode(int id) {
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->ID = id;
    newNode->left = newNode->right = NULL;
    return newNode;
}
// Fungsi untuk menambahkan node ke dalam Tree --> Saat Check-in
BSTNode* insertBSTNode(BSTNode* root, int id) {
    if (root == NULL) {
        return createBSTNode(id);
    }

    if (id < root->ID) {
        root->left = insertBSTNode(root->left, id);
    } else if (id > root->ID) {
        root->right = insertBSTNode(root->right, id);
    }

    return root;
}
// Fungsi untuk mencari node dengan nilai minimum di Tree --> Saat Boarding
BSTNode* findMin(BSTNode* root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}
// Fungsi untuk menghapus node dari BST --> Saat Boarding
BSTNode* deleteBSTNode(BSTNode* root, int id) {
    if (root == NULL) {
        return root;
    }

    if (id < root->ID) {
        root->left = deleteBSTNode(root->left, id);
    } else if (id > root->ID) {
        root->right = deleteBSTNode(root->right, id);
    } else {
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }

        BSTNode* temp = findMin(root->right);
        root->ID = temp->ID;
        root->right = deleteBSTNode(root->right, temp->ID);
    }

    return root;
}

// HASH TABLE
// Inisialisasi tabel hash
void initHashTable(HashTable* ht) {
    for (int i = 0; i < MAX_HASH_SIZE; i++) {
        ht->table[i] = NULL;
    }
}
// Fungsi hash sederhana berdasarkan destinasi
int hashFunction(const char* destinasi) {
    int sum = 0;
    for (int i = 0; destinasi[i] != '\0'; i++) {
        sum += destinasi[i];
    }
    return sum % MAX_HASH_SIZE;
}
// Tambahkan Penumpang ke Hash Table
void addToHashTable(HashTable* ht, Penumpang p) {
    int index = hashFunction(p.tiket.destinasi);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->data = p;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
    printf("Debug: Penumpang dengan ID [%d] telah ditambahkan ke tabel hash.\n", p.ID);
}

//  FUNGSI UNTUK MENU-MENU PILIHAN
//  1) Pesan Tiket --> Tampilkan Destinasi
void tampilkanDestinasiDanWaktu() {
    printf("\nBerikut daftar destinasi yang tersedia dan waktu keberangkatan\n");
    printf("+----+----------------+-----------------------+\n");
    printf("| No |    Destinasi   |   Waktu Keberangkatan |\n");
    printf("+----+----------------+-----------------------+\n");
    for(int i = 0; i < 6; i++) {
        printf("| %-2d | %-14s | %-21s |\n", i+1, listDestinasi[i], waktu[i]);
    }
    printf("+----+----------------+-----------------------+\n");
}
//  1) Pesan Tiket --> Buat Nomor Tiket
void buatNomorTiket(Penumpang* p, int destinasiIndex) {
    snprintf(p->nomorTiket, sizeof(p->nomorTiket), "CT%s-%s", listCode[destinasiIndex], p->tiket.seat);
}
//  1) Pesan Tiket
void pesanTiket() {
    Penumpang p;
    int pilihanDestinasi;

    printf("Masukkan nama penumpang: ");
    getchar();
    fgets(p.nama, sizeof(p.nama), stdin);
    strtok(p.nama, "\n");

    printf("Masukkan nomor paspor: ");
    fgets(p.passport, sizeof(p.passport), stdin);
    strtok(p.passport, "\n");

    tampilkanDestinasiDanWaktu();

    printf("\nPilih destinasi (1-6): ");
    scanf("%d", &pilihanDestinasi);
    strcpy(p.tiket.destinasi, listDestinasi[pilihanDestinasi - 1]);
    strcpy(p.tiket.time, waktu[pilihanDestinasi - 1]);

    printf("Masukkan nomor kursi: ");
    scanf("%s", p.tiket.seat);

    buatNomorTiket(&p, pilihanDestinasi - 1);

    p.ID = penumpangID;
    penumpangID++;

    enqueue(&antrian, p);
    addToHashTable(&hashTable, p);

    printf("\nTiket berhasil dipesan!\n");
    printf("ID Penumpang        : %d\n", p.ID);
    printf("Nama Penumpang      : %s\n", p.nama);
    printf("Nomor Paspor        : %s\n", p.passport);
    printf("Destinasi           : %s\n", p.tiket.destinasi);
    printf("Nomor Kursi         : %s\n", p.tiket.seat);
    printf("Nomor Tiket         : %s\n", p.nomorTiket);
    printf("Waktu Keberangkatan : %s\n", p.tiket.time);
}

//  2) Tampilkan Seluruh Tiket
void tampilkanSeluruhTiket() {
    if (isEmptyQueue(&antrian)) {
        printf("\nMaaf, data penumpang kosong!\n");
        return;
    }

    printf("\nDaftar Tiket Penumpang\n");
    printf("+------+---------------+---------------------+----------------+------+-----------------+-----------------------+\n");
    printf("|  ID  | Paspor        | Nama                | Destinasi      | Seat | Nomor Tiket     | Waktu Keberangkatan   |\n");
    printf("+------+---------------+---------------------+----------------+------+-----------------+-----------------------+\n");

    Node* temp = antrian.head;
    while (temp != NULL) {
        Penumpang p = temp->data;
        printf("| %-4d | %-13s | %-19s | %-14s | %-4s | %-15s | %-21s |\n", p.ID, p.passport, p.nama, p.tiket.destinasi, p.tiket.seat, p.nomorTiket, p.tiket.time);
        temp = temp->next;  
    }

    printf("+------+---------------+---------------------+----------------+------+-----------------+-----------------------+\n");
}

// 3) Tampilkan Daftar Sesuai Destinasi 
void tampilkanDariDestinasi(HashTable* ht, const char* destinasi) {
    int index = hashFunction(destinasi);
    HashNode* temp = ht->table[index];
    int found = 0;

    printf("\n");
    printf("\nPenumpang dengan destinasi    : %s \n", destinasi);
    printf("+------+---------------+---------------------+------+-----------------+-----------------------+\n");
    printf("| ID   |   Paspor      | Nama                | Seat | Nomor Tiket     | Waktu Keberangkatan   |\n");
    printf("+------+---------------+---------------------+------+-----------------+-----------------------+\n");

    while (temp != NULL) {
        if (strcmp(temp->data.tiket.destinasi, destinasi) == 0) {
            Penumpang p = temp->data;
            printf("| %-4d | %-13s | %-19s | %-4s | %-15s | %-21s |\n", p.ID, p.passport, p.nama, p.tiket.seat, p.nomorTiket, p.tiket.time);
            found = 1;
        }
        temp = temp->next;
    }

    if (!found) {
        printf("| %-76s |\n", "Tidak ada penumpang yang ditemukan");
    }

    printf("+------+---------------+---------------------+------+-----------------+-----------------------+\n");
}

//  4) Cari Penumpang Berdasarkan ID
void cariPesanan(Queue* q, int id) {
    if (isEmptyQueue(q)) {
        printf("\nAntrian kosong, tidak ada data penumpang yang bisa dicari.\n");
        return;
    }

    Node* temp = q->head;
    while (temp != NULL) {
        if (temp->data.ID == id) {
            printf("\nData Penerbangan Ditemukan!\n");
            printf("ID Penumpang        : %d\n", temp->data.ID);
            printf("Nama Penumpang      : %s\n", temp->data.nama);
            printf("Nomor Paspor        : %s\n", temp->data.passport);
            printf("Destinasi           : %s\n", temp->data.tiket.destinasi);
            printf("Nomor Kursi         : %s\n", temp->data.tiket.seat);
            printf("Nomor Tiket         : %s\n", temp->data.nomorTiket);
            printf("Waktu Keberangkatan : %s\n", temp->data.tiket.time);
            return;
        }
        temp = temp->next;
    }
    printf("\nPenumpang dengan ID [%d] tidak ditemukan.\n", id);

    getchar();
}

//  5) Perbarui Data Penumpang Berdasarkan ID
void perbaruiPesanan(Queue* q, int id) {
    if (isEmptyQueue(q)) {
        printf("\nAntrian kosong, tidak ada data penumpang yang bisa diperbarui.\n");
        return;
    }

    Node* temp = q->head;
    while (temp != NULL) {
        if (temp->data.ID == id) {
            printf("\nPenumpang Ditemukan! Silakan masukkan data baru.\n");
            printf("Nama Penumpang [%s]: ", temp->data.nama);
            getchar();
            fgets(temp->data.nama, sizeof(temp->data.nama), stdin);
            strtok(temp->data.nama, "\n");
            
            printf("Nomor Paspor [%s]: ", temp->data.passport);
            fgets(temp->data.passport, sizeof(temp->data.passport), stdin);
            strtok(temp->data.passport, "\n");
            
            printf("Nomor Kursi [%s]: ", temp->data.tiket.seat);
            fgets(temp->data.tiket.seat, sizeof(temp->data.tiket.seat), stdin);
            strtok(temp->data.tiket.seat, "\n");
            
            printf("\nData penumpang dengan ID [%d] berhasil diperbarui.\n", id);
            return;
        }
        temp = temp->next;
    }
    printf("\nPenumpang dengan ID [%d] tidak ditemukan.\n", id);
}

// 6) Hapus Tiket
void hapusTiket() {
    if (isEmptyQueue(&antrian)) {
        printf("\nTidak ada tiket untuk dihapus.\n");
        return;
    }

    int id;
    printf("\nMasukkan ID tiket yang ingin dihapus: ");
    scanf("%d", &id);

    Node* temp = antrian.head;
    Node* prev = NULL;
    while (temp != NULL && temp->data.ID != id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("\nID tiket tidak ditemukan.\n");
        return;
    }

    if (prev == NULL) {
        antrian.head = temp->next;
    } else {
        prev->next = temp->next;
    }

    if (temp == antrian.tail) {
        antrian.tail = prev;
    }

    // Push data penumpang yang dihapus ke stack
    push(&deletedStack, temp->data);
    
    free(temp);
    printf("\nTiket berhasil dihapus.\n");
}

//  7) Restore Penerbangan yang Dihapus
void restorePesanan() {
    if (isEmptyStack(&deletedStack)) {
        printf("Tidak ada tiket penerbangan yang dapat dikembalikan.\n");
        return;
    }

    Penumpang p = pop(&deletedStack);
    enqueue(&antrian, p); 
    printf("Tiket penerbangan dengan ID [%d] telah dikembalikan ke antrian.\n", p.ID);
}

//  8) Urutkan Berdasarkan Nama --> tukar
void swapNodes(Node* a, Node* b) {
    Penumpang temp = a->data;
    a->data = b->data;
    b->data = temp;
}
//  8) Urutkan Berdasarkan Nama 
void bubbleSortQueue(Queue* q) {
    int swapped;
    Node *ptr1;
    Node *lptr = NULL;

    if (isEmptyQueue(q))
        return;

    do {
        swapped = 0;
        ptr1 = q->head;

        while (ptr1->next != lptr) {
            if (strcmp(ptr1->data.nama, ptr1->next->data.nama) > 0 || (strcmp(ptr1->data.nama, ptr1->next->data.nama) == 0 && strcmp(ptr1->data.nomorTiket, ptr1->next->data.nomorTiket) > 0)) {
                swapNodes(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

// 9) Check-in & 10) Boarding --> Tampilan antrian 
void preOrderTraversal(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    printf("%d ", root->ID);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}
//  9) Check-in Penumpang
void checkIn() {
    int id;
    printf("Masukkan ID penumpang untuk check-in: ");
    scanf("%d", &id);
    bstRoot = insertBSTNode(bstRoot, id);
    printf("Penumpang dengan ID [%d] berhasil check-in\n", id);
    printf("Penumpang dengan ID [%d] dalam status boarding\n", id);
    printf("Status Boarding (PreOrder): ");
    preOrderTraversal(bstRoot);
    printf("\n");
}
//  10) Boarding
void boarding() {
    int id;
    printf("Antrian Boarding (PreOrder): ");
    preOrderTraversal(bstRoot);
    printf("\n");
    printf("Masukkan ID penumpang : ");
    scanf("%d", &id);
    bstRoot = deleteBSTNode(bstRoot, id);
    printf("Penumpang dengan ID [%d] telah masuk pesawat\n", id);
    printf("\nAntrian Boarding Sekarang (PreOrder): ");
    preOrderTraversal(bstRoot);
    printf("\n");
}

// 11) Cari Penumpang Dalam Daftar Boarding
BSTNode* searchBSTNode(BSTNode* root, int id) {
    if (root == NULL || root->ID == id) {
        return root;
    }

    if (id < root->ID) {
        return searchBSTNode(root->left, id);
    } else {
        return searchBSTNode(root->right, id);
    }
}
//  11) Cari Penumpang Dalam Daftar Boarding --> Tampilkan detail data ID
void cariPenumpangDalamBoarding(Queue* q, int id) {
    BSTNode* foundNode = searchBSTNode(bstRoot, id);
    if (foundNode == NULL) {
        printf("\nPenumpang dengan ID [%d] tidak ditemukan dalam daftar boarding.\n", id);
        return;
    }

    // Cari data penumpang dalam antrian untuk menampilkan informasi lengkap
    Node* temp = q->head;
    while (temp != NULL) {
        if (temp->data.ID == id) {
            printf("\nData Penumpang Ditemukan!\n");
            printf("ID Penumpang        : %d\n", temp->data.ID);
            printf("Nama Penumpang      : %s\n", temp->data.nama);
            printf("Nomor Paspor        : %s\n", temp->data.passport);
            printf("Destinasi           : %s\n", temp->data.tiket.destinasi);
            printf("Nomor Kursi         : %s\n", temp->data.tiket.seat);
            printf("Nomor Tiket         : %s\n", temp->data.nomorTiket);
            printf("Waktu Keberangkatan : %s\n", temp->data.tiket.time);
            return;
        }
        temp = temp->next;
    }
    printf("\nData lengkap penumpang dengan ID [%d] tidak ditemukan dalam antrian.\n", id);
}


//  FUNGSI MAIN
int main() {
    initQueue(&antrian);
    initStack(&deletedStack);
    int pilihan;

    do {
        printf("\n");
        printf("=================================================================\n");
        printf("|                    SISTEM TICKETING PESAWAT                   |\n");
        printf("=================================================================\n");
        printf("|                        MENU PILIHAN:                          |\n");
        printf("|                    1) Pesan Tiket                             |\n");
        printf("|                    2) Tampilkan Seluruh Tiket                 |\n");
        printf("|                    3) Tampilkan Daftar Sesuai Destinasi       |\n");
        printf("|                    4) Cari Penumpang Berdasarkan ID           |\n");
        printf("|                    5) Perbarui Data Penumpang Berdasarkan ID  |\n");
        printf("|                    6) Hapus Penumpang Berdasarkan ID          |\n");
        printf("|                    7) Restore Penerbangan yang dihapus        |\n");
        printf("|                    8) Urutkan Daftar Berdasarkan Nama         |\n");
        printf("|                    9) Check-in                                |\n");
        printf("|                    10) Boarding                               |\n");
        printf("|                    11) Cari Penumpang dalam Daftar Boarding   |\n");
        printf("|                    12) Keluar                                 |\n");
        printf("=================================================================\n");
        printf("                    Masukkan pilihan: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                pesanTiket();
                
                getchar();
                break;
            case 2:
                tampilkanSeluruhTiket();
                
                getchar();
                break;
            case 3:
                char destinasi[MAX_STRING];
                printf("===================================================================\n");
                printf("             Tampilkan Penumpang Berdasarkan Destinasi             \n");
                printf("===================================================================\n");
                tampilkanDestinasiDanWaktu();
                printf("            Masukkan destinasi yang ingin ditampilkan: ");
                getchar();
                fgets(destinasi, sizeof(destinasi), stdin);
                strtok(destinasi, "\n");
                tampilkanDariDestinasi(&hashTable, destinasi);
                break;            
            case 4: {
                int id;
                printf("===================================================================\n");
                printf("                    Cari Pesanan Tiket Penumpang                   \n");
                printf("===================================================================\n");
                printf("            Masukkan ID penumpang yang ingin dicari: ");
                scanf("%d", &id);
                cariPesanan(&antrian, id);
                break;
            }
            case 5: {
                int id;
                printf("===================================================================\n");
                printf("                   Perbarui Data Penumpang Berdasarkan ID          \n");
                printf("===================================================================\n");
                printf("             Masukkan ID penumpang yang ingin diperbarui: ");
                scanf("%d", &id);
                perbaruiPesanan(&antrian, id);
                break;
            }
            case 6: {
                printf("===================================================================\n");
                printf("                   Hapus Pesanan Tiket Penumpang                   \n");
                printf("===================================================================\n");
                hapusTiket(&antrian);
                break;
            }
            case 7: {
                printf("===================================================================\n");
                printf("                 Restore Penerbangan yang dihapus                  \n");
                printf("===================================================================\n");
                restorePesanan();
                break;                
            }
            case 8: {
                printf("===================================================================\n");
                printf("           Daftar tiket telah diurutkan berdasarkan nama           \n");
                printf("===================================================================\n");
                bubbleSortQueue(&antrian);
                break;                
            }
            case 9:
                printf("===================================================================\n");
                printf("                          Check-In Penumpang                       \n");
                printf("===================================================================\n");
                checkIn();
                break;
            case 10:
                printf("===================================================================\n");
                printf("                           Antrian Boarding                        \n");
                printf("===================================================================\n");
                boarding();
                break;
            case 11: {
                int id;
                printf("===================================================================\n");
                printf("                Cari Penumpang dalam Daftar Boarding               \n");
                printf("===================================================================\n");
                printf("            Masukkan ID penumpang yang ingin dicari: ");
                scanf("%d", &id);
                cariPenumpangDalamBoarding(&antrian, id);
                break;
            }
            case 12:
                printf("===================================================================\n");
                printf("                            TERIMA KASIH                           \n");
                printf("===================================================================\n");
                printf("Keluar dari program.\n");
                getchar();
                break;
            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
                break;
        }
    } while (pilihan != 12);

    return 0;
}