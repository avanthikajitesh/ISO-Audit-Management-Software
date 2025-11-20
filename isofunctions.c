// miniproject_functions.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iso.h"

// --------------- Constants ---------------
AuditTypePayment paymentTable[AUDIT_TYPE_COUNT] = {0};

// --------------- Stack ---------------
StackNode* push(StackNode* top, AuditRecord rec) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->record = rec;
    newNode->next = top;
    return newNode;
}

StackNode* pop(StackNode** top) {
    if (*top == NULL) return NULL;
    StackNode* temp = *top;
    *top = (*top)->next;
    return temp;
}

// --------------- Queue ---------------
void enqueue(QueueNode** front, QueueNode** rear, AuditRecord rec) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->record = rec;
    newNode->next = NULL;
    if (*rear == NULL) {
        *front = *rear = newNode;
        return;
    }
    (*rear)->next = newNode;
    *rear = newNode;
}

QueueNode* dequeue(QueueNode** front, QueueNode** rear) {
    if (*front == NULL) return NULL;
    QueueNode* temp = *front;
    *front = (*front)->next;
    if (*front == NULL) *rear = NULL;
    return temp;
}

// --------------- BST ---------------
int compareDate(char* d1, char* d2) {
    return strcmp(d1, d2);
}

BSTNode* insertBST(BSTNode* root, AuditRecord rec) {
    if (root == NULL) {
        BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
        node->record = rec;
        node->left = node->right = NULL;
        return node;
    }
    if (compareDate(rec.date, root->record.date) < 0)
        root->left = insertBST(root->left, rec);
    else
        root->right = insertBST(root->right, rec);
    return root;
}

void inorderBST(BSTNode* root) {
    if (root == NULL) return;
    inorderBST(root->left);
    printf("ID:%d | Date:%s | Dept:%s | CompanyType:%s | Category:%s | AuditType:%s | Payment:%.2lf\n",
           root->record.auditID, root->record.date, root->record.department,
           root->record.companyType, root->record.category,
           root->record.auditType, root->record.payment);
    inorderBST(root->right);
}

/* New: find record by auditID; returns pointer to record inside BSTNode, or NULL */
AuditRecord* findRecordByID(BSTNode* root, int auditID) {
    if (root == NULL) return NULL;
    if (root->record.auditID == auditID) return &root->record;
    AuditRecord* left = findRecordByID(root->left, auditID);
    if (left) return left;
    return findRecordByID(root->right, auditID);
}

// --------------- Linked List ---------------
void addOngoingAudit(ListNode** head, AuditRecord rec) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    newNode->record = rec;
    newNode->next = *head;
    *head = newNode;
}

void removeOngoingAudit(ListNode** head, int auditID) {
    ListNode *temp = *head, *prev = NULL;
    while (temp && temp->record.auditID != auditID) {
        prev = temp;
        temp = temp->next;
    }
    if (!temp) return;
    if (!prev) *head = temp->next;
    else prev->next = temp->next;
    free(temp);
}

void printOngoingAudits(ListNode* head) {
    if (head == NULL) {
        printf("No ongoing audits.\n");
        return;
    }
    while (head) {
        printf("ID:%d | Dept:%s | Payment:%.2lf\n",
               head->record.auditID, head->record.department, head->record.payment);
        head = head->next;
    }
}

// --------------- Priority/Heap ---------------
PriorityQueue* createPriorityQueue(int capacity) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->heapArray = (AuditRecord*)malloc(capacity * sizeof(AuditRecord));
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void swap(AuditRecord* a, AuditRecord* b) {
    AuditRecord temp = *a;
    *a = *b;
    *b = temp;
}

double computePriority(AuditRecord rec) {
    for (int i = 0; i < AUDIT_TYPE_COUNT; i++) {
        if (strcmp(rec.auditType, paymentTable[i].auditType) == 0) {
            return paymentTable[i].priorityValue;
        }
    }
    return 0;
}

void maxHeapify(PriorityQueue* pq, int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;
    if (left < pq->size && computePriority(pq->heapArray[left]) > computePriority(pq->heapArray[largest]))
        largest = left;
    if (right < pq->size && computePriority(pq->heapArray[right]) > computePriority(pq->heapArray[largest]))
        largest = right;
    if (largest != i) {
        swap(&pq->heapArray[i], &pq->heapArray[largest]);
        maxHeapify(pq, largest);
    }
}

void insertPQ(PriorityQueue* pq, AuditRecord rec) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->heapArray = (AuditRecord*)realloc(pq->heapArray, pq->capacity * sizeof(AuditRecord));
    }
    int i = pq->size++;
    pq->heapArray[i] = rec;
    while (i != 0 && computePriority(pq->heapArray[(i-1)/2]) < computePriority(pq->heapArray[i])) {
        swap(&pq->heapArray[i], &pq->heapArray[(i-1)/2]);
        i = (i-1)/2;
    }
}

AuditRecord extractMax(PriorityQueue* pq) {
    if (pq->size <=0) {
        AuditRecord empty = {0};
        return empty;
    }
    AuditRecord root = pq->heapArray[0];
    pq->heapArray[0] = pq->heapArray[--pq->size];
    maxHeapify(pq, 0);
    return root;
}

void printPriorityQueue(PriorityQueue* pq) {
    if (pq->size == 0) {
        printf("\nNo scheduled audits in priority queue.\n");
        return;
    }

    AuditRecord* tempArr = (AuditRecord*)malloc(pq->size * sizeof(AuditRecord));
    int tempSize = pq->size;
    for (int i = 0; i < tempSize; i++) tempArr[i] = pq->heapArray[i];

    // Sort by priority (descending)
    for (int i = 0; i < tempSize-1; i++) {
        int maxIdx = i;
        double maxPriority = computePriority(tempArr[i]);
        for (int j = i+1; j < tempSize; j++) {
            double curPriority = computePriority(tempArr[j]);
            if (curPriority > maxPriority) {
                maxIdx = j;
                maxPriority = curPriority;
            }
        }
        if (maxIdx != i) {
            AuditRecord tmp = tempArr[i];
            tempArr[i] = tempArr[maxIdx];
            tempArr[maxIdx] = tmp;
        }
    }

    printf("\n--- Audits by Priority (Highest to Lowest) ---\n");
    for(int i=0; i<tempSize; i++) {
        printf("ID:%d | Dept:%s | Type:%s | Payment:%.2lf | Priority Level:%.1lf\n",
            tempArr[i].auditID, tempArr[i].department, tempArr[i].auditType,
            tempArr[i].payment, computePriority(tempArr[i]));
    }
    free(tempArr);
}

// --------------- Input Validation ---------------

int nextAuditID = 1; // start from 1 → becomes 001

int isValidDate(char *date) {
    int y, m, d;

    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    sscanf(date, "%d-%d-%d", &y, &m, &d);

    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;

    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    // leap year
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
        daysInMonth[1] = 29;

    return d <= daysInMonth[m - 1];
}

void loadPricingFromCSV(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("ERROR: Unable to open pricing CSV file: %s\n", filename);
        return;
    }

    char line[200];
    int i = 0;

    while (fgets(line, sizeof(line), fp) && i < AUDIT_TYPE_COUNT) {
        char type[20];
        double payment, priority;

        if (sscanf(line, "%[^,],%lf,%lf", type, &payment, &priority) == 3) {
            strcpy(paymentTable[i].auditType, type);
            paymentTable[i].payment = payment;
            paymentTable[i].priorityValue = priority;
            i++;
        }
    }

    fclose(fp);

    /* Optional debug print: uncomment to verify CSV loaded correctly */
    /*
    for (int j = 0; j < AUDIT_TYPE_COUNT; j++) {
        printf("Loaded pricing: %s -> payment: %.2lf, priority: %.2lf\n",
               paymentTable[j].auditType, paymentTable[j].payment, paymentTable[j].priorityValue);
    }
    */
}


AuditRecord inputAuditRecord() {
    AuditRecord rec;
    int valid = 0, auditTypeIndex = -1;

    // Auto-generate sequential Audit ID
    rec.auditID = nextAuditID++;
    printf("Generated Audit ID: %04d\n", rec.auditID);  // prints 001, 002, 003...

    // Initialize payment default to 0 to be safe
    rec.payment = 0.0;

    // Department
    do {
        printf("Select Department:\n");
        printf("1. HR\n");
        printf("2. Finance\n");
        printf("3. Operations\n");
        printf("4. IT\n");
        printf("Enter choice (1-4): ");

        int deptChoice;
        if (scanf("%d", &deptChoice) != 1) {
            // flush invalid input
            while (getchar() != '\n');
            printf("Invalid input. Try again.\n");
            valid = 0;
            continue;
        }

        valid = 1;
        switch (deptChoice) {
            case 1: strcpy(rec.department, "HR"); break;
            case 2: strcpy(rec.department, "Finance"); break;
            case 3: strcpy(rec.department, "Operations"); break;
            case 4: strcpy(rec.department, "IT"); break;
            default:
                printf("Invalid department. Try again.\n");
                valid = 0;
        }

    } while (!valid);


    // Date
    do {
        printf("Enter Date (YYYY-MM-DD): ");
        scanf(" %10s", rec.date);
        valid = isValidDate(rec.date);
        if (!valid) printf("Invalid date format.\n");
    } while (!valid);

    // Company Type
    do {
        printf("Enter Company Type (MNC or Non-MNC): ");
        scanf(" %9s", rec.companyType);
        valid = (strcmp(rec.companyType,"MNC")==0 || strcmp(rec.companyType,"Non-MNC")==0);
        if (!valid) printf("Must be MNC or Non-MNC.\n");
    } while(!valid);

    // Category if MNC
    if(strcmp(rec.companyType, "MNC")==0) {
        do {
            printf("Enter Category (Product, Service, Location): ");
            scanf(" %19s", rec.category);
            valid = (strcmp(rec.category,"Product")==0 || strcmp(rec.category,"Service")==0 || strcmp(rec.category,"Location")==0);
            if(!valid) printf("Invalid category for MNC.\n");
        } while(!valid);
    } else {
        strcpy(rec.category,"N/A");
    }

    // Audit Type: must be one of Standard, Medium, Fastest
    do {
        printf("Enter Audit Type (Standard, Medium, Fastest): ");
        scanf(" %9s", rec.auditType);
        valid = 0;
        for(int i=0; i<AUDIT_TYPE_COUNT; i++) {
            if(strcmp(rec.auditType, paymentTable[i].auditType)==0) {
                auditTypeIndex = i;
                valid = 1;
                /* --- IMPORTANT: store the payment into the record --- */
                rec.payment = paymentTable[i].payment;
                printf("Payment for %s audit MUST be %.0lf\n", rec.auditType, paymentTable[i].payment);
                break;
            }
        }
        if(!valid) printf("Invalid audit type.\n");
    } while(!valid);

    return rec;
}
