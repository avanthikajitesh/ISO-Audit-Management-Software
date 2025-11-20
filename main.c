// miniproject_main.c

#include <stdio.h>
#include <stdlib.h>
#include "iso.h"


int main() {
    StackNode* undoStack = NULL;
    QueueNode* front = NULL;
    QueueNode* rear = NULL;
    BSTNode* bstRoot = NULL;
    ListNode* ongoingHead = NULL;
    PriorityQueue* pq = createPriorityQueue(10);
    int choice;
    AuditRecord rec;

    char filename[100]; 
    printf("Enter CSV file name for pricing: ");
    loadPricingFromCSV((scanf("%99s", filename), filename));

    while(1) {
        printf("\n===== ISO Audit Management =====\n");
        printf("1. Add Audit Record\n");
        printf("2. Schedule Audit\n");
        printf("3. Process Audit Request\n");
        printf("4. Undo Last Added Audit Record\n");
        printf("5. View Audit Records by Date\n");
        printf("6. Add Ongoing Audit\n");
        printf("7. View Ongoing Audits\n");
        printf("8. View All Scheduled Audits by Priority\n");
        printf("9. Remove Ongoing Audit by ID\n");
        printf("0. Exit\n");

        while (1) {
            printf("Choice: ");
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input! Please enter a valid number.\n");
                while (getchar() != '\n'); // flush invalid characters
            } else {
                break;
            }
        }

        switch(choice) {
            case 1: {
                /* Option 1: ONLY create a new audit record and store it in BST + undo stack */
                rec = inputAuditRecord();
                undoStack = push(undoStack, rec);
                bstRoot = insertBST(bstRoot, rec);
                printf("Added Audit Record and Undo Stack updated.\n");
                break;
            }
            case 2: {
                /* Option 2: Schedule Audit - offer to create new and schedule OR move existing to schedule */
                int opt;
                printf("\nSchedule Audit - Choose:\n1. Create New & Schedule\n2. Move Existing Record to Schedule\nChoice: ");
                if (scanf("%d", &opt) != 1) {
                    while (getchar() != '\n');
                    printf("Invalid input.\n");
                    break;
                }
                if (opt == 1) {
                    rec = inputAuditRecord();
                    enqueue(&front, &rear, rec);
                    insertPQ(pq, rec);
                    /* Also add to BST/undo since it's a new added record */
                    undoStack = push(undoStack, rec);
                    bstRoot = insertBST(bstRoot, rec);
                    printf("New audit created and scheduled.\n");
                } else if (opt == 2) {
                    if (bstRoot == NULL) {
                        printf("No existing records. Add a new one first (Option 1).\n");
                        break;
                    }
                    printf("\nExisting Audit Records:\n");
                    inorderBST(bstRoot);
                    int id;
                    printf("Enter Audit ID to move to Schedule: ");
                    if (scanf("%d", &id) != 1) {
                        while (getchar() != '\n');
                        printf("Invalid input.\n");
                        break;
                    }
                    AuditRecord* found = findRecordByID(bstRoot, id);
                    if (!found) {
                        printf("Audit ID %d not found.\n", id);
                        break;
                    }
                    enqueue(&front, &rear, *found);
                    insertPQ(pq, *found);
                    printf("Audit ID %d moved to Schedule.\n", found->auditID);
                } else {
                    printf("Invalid option.\n");
                }
                break;
            }
            case 3: {
                QueueNode* qnode = dequeue(&front, &rear);
                if(qnode) {
                    printf("Processing Audit ID: %d Dept: %s\n", qnode->record.auditID, qnode->record.department);
                    // Optionally, add to ongoing audits here
                    free(qnode);
                } else {
                    printf("Queue is empty.\n");
                }
                break;
            }
            case 4: {
                StackNode* undone = pop(&undoStack);
                if(undone) {
                    printf("Undo Audit ID: %d Dept: %s\n", undone->record.auditID, undone->record.department);
                    free(undone);
                } else {
                    printf("Nothing to undo.\n");
                }
                break;
            }
            case 5:
                printf("\nAudit Records Sorted by Date:\n");
                inorderBST(bstRoot);
                break;
            case 6: {
                /* Option 6: Add Ongoing Audit - offer create new OR move existing */
                int opt;
                printf("\nAdd Ongoing Audit - Choose:\n1. Create New & Add Ongoing\n2. Move Existing Record to Ongoing\nChoice: ");
                if (scanf("%d", &opt) != 1) {
                    while (getchar() != '\n');
                    printf("Invalid input.\n");
                    break;
                }
                if (opt == 1) {
                    rec = inputAuditRecord();
                    addOngoingAudit(&ongoingHead, rec);
                    /* Also add to BST/undo since it's a new added record */
                    undoStack = push(undoStack, rec);
                    bstRoot = insertBST(bstRoot, rec);
                    printf("New audit created and added to Ongoing.\n");
                } else if (opt == 2) {
                    if (bstRoot == NULL) {
                        printf("No existing records. Add a new one first (Option 1).\n");
                        break;
                    }
                    printf("\nExisting Audit Records:\n");
                    inorderBST(bstRoot);
                    int id;
                    printf("Enter Audit ID to move to Ongoing: ");
                    if (scanf("%d", &id) != 1) {
                        while (getchar() != '\n');
                        printf("Invalid input.\n");
                        break;
                    }
                    AuditRecord* found = findRecordByID(bstRoot, id);
                    if (!found) {
                        printf("Audit ID %d not found.\n", id);
                        break;
                    }
                    addOngoingAudit(&ongoingHead, *found);
                    printf("Audit ID %d added to Ongoing.\n", found->auditID);
                } else {
                    printf("Invalid option.\n");
                }
                break;
            }
            case 7:
                printf("\nOngoing Audits:\n");
                printOngoingAudits(ongoingHead);
                break;
            case 8:
                printPriorityQueue(pq);
                break;
            case 9: {
                int id;
                printf("Enter Audit ID to remove from ongoing audits: ");
                scanf("%d", &id);
                removeOngoingAudit(&ongoingHead, id);
                printf("Removed if existed.\n");
                break;
            }
            case 0:
                printf("Exiting Program.\n");
                return 0;
            default:
                printf("Invalid Choice.\n");
        }
    }
    return 0;
}
