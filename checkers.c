/*
# Description: 
# Author: Xin Yu
# Date: 05 November 2021
*/

/* -------------------------------------------------------------------------- */
/*                                   HEADER                                   */
/* -------------------------------------------------------------------------- */

/* -------------------------------- #includes ------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/* -------------------------------- #defines -------------------------------- */
#define TITLE "_________ .__                   __         \n\
\\_   ___ \\|  |__   ____   ____ |  | __ ___________  ______\n\
/    \\  \\/|  |  \\_/ __ \\_/ ___\\|  |/ // __ \\_  __ \\/  ___/\n\
\\     \\___|   Y  \\  ___/\\  \\___|    <\\  ___/|  | \\/\\___ \\ \n\
 \\______  /___|  /\\___  >\\___  >__|_ \\___  >__|  /____  >\n\
        \\/     \\/     \\/     \\/     \\/    \\/          \\/ \n"

// board
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces

// cell values
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character

// minimax
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth

#define P_COMP_ACTIONS      10      // number of computed actions for P command
#define A_COMP_ACTIONS      1       // number of computed actions for A command

// inputting
#define NEWLINE_CHAR        '\n'    // marks the end of an action or command
#define DASH                '-'
#define ACTION_LEN          4
#define COMMAND_LEN         1
#define TEMP                "XXXX"  // temporary action placeholder

// action array positions
#define SRC_COL             0
#define SRC_ROW             1
#define TAR_COL             2
#define TAR_ROW             3

// formatting and printing
#define ROWS_WO_PIECES      2
#define DIVIDER             "=====================================\n"
#define NEWLINE             "\n"
#define COL_DISPLAY         "     A   B   C   D   E   F   G   H\n"
#define CELL_DIVIDER        "|"
#define ROW_DIVIDER         "   +---+---+---+---+---+---+---+---+\n"
#define COMPUTED_MARKER     "*** "

// players
#define NUM_PLAYERS         2
#define BLACK               1
#define WHITE               0

// booleans
#define FALSE               0
#define TRUE                1

// offsets
#define ROW_OFFSET          -49     // convert to grid index value ('1' to 0)
#define COL_OFFSET          -65     // convert to grid index value ('A' to 0)

// action movement
#define WEST                -1
#define EAST                1
#define NORTH               -1
#define SOUTH               1
#define CAP_STEP            2       // number of steps when capturing
#define MOVE_STEP           1       // number of steps when making a normal move

// row for pieces to turn to towers
#define B_TOW_ROW           '1'     // row for black pieces turn into towers
#define W_TOW_ROW           '8'     // row for white pieces turn into towers

// command characters
#define A_COMMAND           'A'
#define P_COMMAND           'P'

// minimax
#define ROOT_DEPTH          0       // the root depth

// error messages
#define ERROR1              "ERROR: Source cell is outside of the board.\n"
#define ERROR2              "ERROR: Target cell is outside of the board.\n"
#define ERROR3              "ERROR: Source cell is empty.\n"
#define ERROR4              "ERROR: Target cell is not empty.\n"
#define ERROR5              "ERROR: Source cell holds opponent's piece/tower.\n"
#define ERROR6              "ERROR: Illegal action.\n"

// winning messages
#define WHITE_WIN           "WHITE WIN!\n"
#define BLACK_WIN           "BLACK WIN!\n"

/* -------------------------------- typedefs -------------------------------- */
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type
typedef char action_t[ACTION_LEN+1];
typedef struct node node_t;
struct node {
    action_t    action;
    int         cost;
    node_t      *next;
};
typedef struct {
    node_t      *head;
    node_t      *foot;
} list_t;

/* --------------------------- Function Prototypes -------------------------- */
// functions used for inputting and printing
int get_action(action_t action, char *command);
void initial_setup(board_t board);
void print_board(board_t board);
void print_action(action_t action, int turn_count, board_t board,
    int is_computed);
int count_type(board_t board, char type);
int calculate_cost(board_t board);

// functions related to doing or checking actions
int is_action_legal(board_t board, action_t action, int player,
    int is_printing);
void do_action(board_t board, action_t action, int player);

// functions related to minimax
node_t *minimax(int depth, int max_depth, board_t board, int player, 
    node_t *node);
list_t *get_valid_actions(board_t board, int player, list_t *valid_actions);
list_t *get_valid_action_in_dir(int col, int row, int y, int x, int player, 
    board_t board, list_t *valid_actions);
void copy_board(board_t board, board_t board_copy);

// functions related to linked lists
node_t *create_new_node(char *action);
list_t *make_empty_list(void);
list_t *insert_at_foot(list_t *list, node_t *new);
void free_list(list_t *list);

/* ------------------------------ Main Function ----------------------------- */
int main (int argc, char *argv[]) {
    printf(TITLE);

    // Read in, execute and print actions
    action_t action;
    char command = DASH;    // using the dash as a placeholder
    int turn = 1;
    
    // 
    printf("Choose mode ([1] Play with bot, [2] Simulate game): ");
    int mode;
    if (scanf("%d", &mode) == 1) {
        while (!(mode == 1 || mode == 2)) {
            printf("Invalid value. Try again. Choose mode [1-Play with bot, \
            2-Bulk input mode): ");
            scanf("%d", &mode);
        }
    }
    if (mode == 1) {
        printf(NEWLINE);
        printf("=================== PLAY WITH BOT MODE ====================\n");
        printf("  * Human - White Pieces ('W' and 'w')\n"
                "  * Bot (with MiniMax tree depth %d) - Black Pieces"
                " ('B' and 'b')\n", TREE_DEPTH);

        // Initial board setup and printing
        board_t board;
        initial_setup(board);
        int game_flag = TRUE;
        
        // Game Loop
        while (game_flag) {
            int player = turn % NUM_PLAYERS;
            if (player == WHITE) {  // human's turn
                printf("Human (White Pieces) Turn - Enter your action"
                " (eg. B4-C4): ");
                get_action(action, &command);
                while (!is_action_legal(board, action, player, TRUE)) {
                    printf("Please try again. Enter your action (eg. B4-C4): ");
                    get_action(action, &command);
                }
                do_action(board, action, player);
                print_action(action, turn, board, FALSE);
                turn++;
            } else {    // bot's turn
                // Find the best action for the current board
                node_t *best_action = create_new_node(TEMP); 
                        // starts with a temporary placeholder action
                best_action = minimax(ROOT_DEPTH, TREE_DEPTH, board, 
                    turn%NUM_PLAYERS, best_action);
                        // that is then be replaced by the best action from minimax
                
                // Check if a player won
                if (best_action->cost == INT_MAX) {
                    printf(BLACK_WIN);
                    game_flag = FALSE;
                    break;
                } else if (best_action->cost == INT_MIN) {
                    printf(WHITE_WIN);
                    game_flag = FALSE;
                    break;
                }

                // Perform the best action on the board and print it
                do_action(board, best_action->action, turn%NUM_PLAYERS);
                print_action(best_action->action, turn, board, TRUE);

                free(best_action);
                turn++;
                
            }
        }
    }

    

    if (mode == 2) {
        printf("===================== BULK INPUT MODE =====================\n");
        printf("Enter a list of actions (eg. B4-C4) followed by a command \
        [A-predict next move, P-predict next 10 moves]:");
        printf(NEWLINE);

        // Initial board setup and printing
        board_t board;
        initial_setup(board);      

        // Read in the list of actions and the command
        while (get_action(action, &command)) {
            int player = turn % NUM_PLAYERS;
            if (!is_action_legal(board, action, player, TRUE)) {
                return EXIT_FAILURE;             // terminate at illegal actions
            }
            do_action(board, action, player);
            print_action(action, turn, board, FALSE);
            turn++;
        }

        // no command inputted
        if (command == DASH) {
            return EXIT_SUCCESS;
        }

        // command inputted- determine the number of prediction moves to compute
        int comp_actions = 0;
        if (command == A_COMMAND) {
            comp_actions = A_COMP_ACTIONS;
        } else if (command == P_COMMAND) {
            comp_actions = P_COMP_ACTIONS;
        }

        for (int i=0; i<comp_actions; i++) {
            // Find the best action for the current board
            node_t *best_action = create_new_node(TEMP); 
                    // starts with a temporary placeholder action
            best_action = minimax(ROOT_DEPTH, TREE_DEPTH, board, 
                turn%NUM_PLAYERS, best_action);
                    // that is then be replaced by the best action from minimax
            
            // Check if a player won
            if (best_action->cost == INT_MAX) {
                printf(BLACK_WIN);
                break;
            } else if (best_action->cost == INT_MIN) {
                printf(WHITE_WIN);
                break;
            }

            // Perform the best action on the board and print it
            do_action(board, best_action->action, turn%NUM_PLAYERS);
            print_action(best_action->action, turn, board, TRUE);

            free(best_action);
            turn++;
        }
    }
    

    return EXIT_SUCCESS;            // exit program with the success code
}

/* ---------------------- Input and Printing Functions ---------------------- */
/* Fills and prints the initial board setup as specified in Figure 1(a) in the
    specification sheet.
*/
void
initial_setup(board_t board) {
    // traverse the board
    for (int row=0; row<BOARD_SIZE; row++) {
        for (int col=0; col<BOARD_SIZE; col++){
            if (col % 2 == ((row + 1) % 2)) {  // checkered cells
                if (row < ROWS_WITH_PIECES) {
                    // fill first three rows with white pieces
                    board[row][col] = CELL_WPIECE;
                } else if (row >= ROWS_WITH_PIECES + ROWS_WO_PIECES) {
                    // last three rows with black pieces
                    board[row][col] = CELL_BPIECE;
                } else {
                    // middle two rows with empty pieces
                    board[row][col] = CELL_EMPTY;
                }
            } else {  // non-checkered cells are empty
                board[row][col] = CELL_EMPTY;
            }
        }
    }
    
    // find number of black and white pieces
    int num_black = count_type(board, CELL_BPIECE);
    int num_white = count_type(board, CELL_WPIECE);

    // print the board
    printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
    printf("#BLACK PIECES: %d\n", num_black);
    printf("#WHITE PIECES: %d\n", num_white);
    print_board(board);
}

/* Extracts an action or command from stdin and stores it in action or command.
    Returns 1 when an action or command is read and 0 if the end of file
    is reached.
*/
int
get_action(action_t action, char *command) {
    char ch;
    int ch_count = 0;
    while ((ch=getchar()) != EOF) {
        if (ch == NEWLINE_CHAR) {  // end of action or command
            if (ch_count == ACTION_LEN) {
                action[ACTION_LEN] = '\0';
                return 1;
            } else if (ch_count == COMMAND_LEN) {
                *command = action[0];
                return 1;
            }
            ch_count = 0;
        } else if (ch == DASH) {  // ignore dashes
            continue;
        } else {
            action[ch_count++] = ch;  // fill in the action string
        }
    }
    return 0;  // end of file reached
}

/* Prints the action information and the checkers board.
*/
void
print_action(action_t action, int turn_count, board_t board, int is_computed) {
    printf(DIVIDER);
    if (is_computed) printf(COMPUTED_MARKER);
    if (turn_count%NUM_PLAYERS) {
        printf("BLACK ACTION #%d: ", turn_count);
    } else {
        printf("WHITE ACTION #%d: ", turn_count);
    }
    printf("%c%c-%c%c\n", action[SRC_COL], action[SRC_ROW], action[TAR_COL],
        action[TAR_ROW]);
    printf("BOARD COST: %d\n", calculate_cost(board));
    print_board(board);
}

/* ------------------- Input and Printing Helper Functions ------------------ */

/* Prints the `board` using stdout.
*/
void
print_board(board_t board) {
    printf(COL_DISPLAY);
    printf(ROW_DIVIDER);
    for (int i=0; i<BOARD_SIZE; i++) {  // traverse the rows
        printf(" %d ", i+1);  // print row number

        for (int j=0; j<BOARD_SIZE; j++){  // traverse the columns
            printf(CELL_DIVIDER);
            printf(" %c ", board[i][j]);  // display cell value
        }
        printf(CELL_DIVIDER);
        printf(NEWLINE);
        printf(ROW_DIVIDER);
    }
}

/* Counts and returns the number of pieces or towers of `type` on the board.
    `type` can be CELL_BPIECE, CELL_WPIECE, CELL_BTOWER or CELL_WTOWER.
*/
int
count_type(board_t board, char type) {
    int num = 0;
    // traverse through the board
    for (int i=0; i<BOARD_SIZE; i++){
        for (int j=0; j<BOARD_SIZE; j++){
            if (board[i][j] == type){  // check if the cell value is of `type`
                num ++;
            }
        }
    }
    return num;
}

/* Calculates and returns the cost of the board.
*/
int
calculate_cost(board_t board) {
    int cost = COST_PIECE * count_type(board, CELL_BPIECE)
             - COST_PIECE * count_type(board, CELL_WPIECE)
             + COST_TOWER * count_type(board, CELL_BTOWER)
             - COST_TOWER * count_type(board, CELL_WTOWER);
    return cost;
}

/* ---------------------------- Action Functions ---------------------------- */

/* Checks if an action is valid. Returns TRUE if the action is legal and FALSE
    otherwise. Also prints error messages for illegal actions if `is_printing`
    is TRUE.
*/
int
is_action_legal(board_t board, action_t action, int player, int is_printing) {
    // (1) source cell is within the board's range
    if (!(action[SRC_COL] >= 'A' && action[SRC_COL] <= 'H') 
        || !(action[SRC_ROW] >= '1' && action[SRC_ROW] <= '8')) {
        if (is_printing) printf(ERROR1);
        return FALSE;
    }

    // (2) target cell is within the board's range
    if (!(action[TAR_COL] >= 'A' && action[TAR_COL] <= 'H') 
        || !(action[TAR_ROW] >= '1' && action[TAR_ROW] <= '8')) {
        if (is_printing) printf(ERROR2);
        return FALSE;
    }

    char src = board[action[SRC_ROW]+ROW_OFFSET][action[SRC_COL]+COL_OFFSET];
    char tar = board[action[TAR_ROW]+ROW_OFFSET][action[TAR_COL]+COL_OFFSET];

    // (3) source cell is empty
    if (src == CELL_EMPTY) {
        if (is_printing) printf(ERROR3);
        return FALSE;
    }

    // (4) target cell is not empty
    if (tar != CELL_EMPTY) {
        if (is_printing) printf(ERROR4);
        return FALSE;
    }

    // (5) source cell holds opponent's piece/tower
    if (((player == BLACK && (src == CELL_WPIECE || src == CELL_WTOWER)) || 
        (player == WHITE && (src == CELL_BPIECE || src == CELL_BTOWER)))) {
        if (is_printing) printf(ERROR5);
        return FALSE;
    }

    // (6a) pieces moving backward
    int row_diff = action[TAR_ROW] - action[SRC_ROW];

    if ((row_diff > 0 && src == CELL_BPIECE)        // black piece moving south
        || (row_diff < 0 && src == CELL_WPIECE)) { // white piece moving north
        if (is_printing) printf(ERROR6);
        // printf("moving backward\n"); // DEBUG
        return FALSE;
    }

    // (6b) piece/tower is not moving diagonally, moving too many steps or
    //      is not moving
    int col_diff = action[TAR_COL] - action[SRC_COL];

    if ((abs(row_diff) != abs(col_diff))        
        || (abs(row_diff) > CAP_STEP)           // too many steps
        || (abs(row_diff) < MOVE_STEP)) {       // too few steps
        if (is_printing) printf(ERROR6);
        return FALSE;
    }

    // (6c) check if the captured cell holds opponent's piece/tower
    if (abs(row_diff) == CAP_STEP) {    // action is a capture
        int cap_col = action[SRC_COL] + col_diff/2 + COL_OFFSET;
        int cap_row = action[SRC_ROW] + row_diff/2 + ROW_OFFSET;
        char cap = board[cap_row][cap_col];
        if (!((player == BLACK && (cap == CELL_WPIECE || cap == CELL_WTOWER))
            || (player == WHITE && (cap == CELL_BPIECE || cap == CELL_BTOWER))
        )) {
            if (is_printing) printf(ERROR6);
            return FALSE;
        }
    }

    return TRUE;
}

/* Performs the action onto the board.
*/
void
do_action(board_t board, action_t action, int player) {
    // Update the target cell
    if (action[TAR_ROW] == B_TOW_ROW || action[TAR_ROW] == W_TOW_ROW) {
        // piece turning into a tower
        if (player == BLACK) {
            board[action[TAR_ROW]+ROW_OFFSET][action[TAR_COL]+COL_OFFSET] =
                CELL_BTOWER;
        } else {
            board[action[TAR_ROW]+ROW_OFFSET][action[TAR_COL]+COL_OFFSET] =
                CELL_WTOWER;
        }
    } else {
        board[action[TAR_ROW]+ROW_OFFSET][action[TAR_COL]+COL_OFFSET] = 
            board[action[SRC_ROW]+ROW_OFFSET][action[SRC_COL]+COL_OFFSET];
    }

    // Update the source cell
    board[action[SRC_ROW]+ROW_OFFSET][action[SRC_COL]+COL_OFFSET] = CELL_EMPTY;

    // Update captured cell
    int row_diff = action[TAR_ROW] - action[SRC_ROW];
    if (abs(row_diff) == CAP_STEP) {  // check if the action is a capture
        int col_diff = action[TAR_COL] - action[SRC_COL];
        int cap_col = action[SRC_COL] + col_diff/2 + COL_OFFSET;
        int cap_row = action[SRC_ROW]+ row_diff/2 + ROW_OFFSET;

        board[cap_row][cap_col] = CELL_EMPTY;
    }
}

/* ---------------------------- Minimax Function ---------------------------- */

/* Recursive function that applies the minimax algorithm till a specific `depth`
    to the current `board` and returns the best action for that board.
*/
node_t
*minimax(int depth, int max_depth, board_t board, int player, node_t *node) {
    // Terminate if leaf node is reached
    if (depth == max_depth) {
        node->cost = calculate_cost(board);
        return node;
    }
    
    // Set different node costs and opponents depending on player
    int opp;
    if (player == BLACK) {  // maimising player
        node->cost = INT_MIN;
        opp = WHITE;
    } else {                // minimising player
        node->cost = INT_MAX;
        opp = BLACK;
    }

    // Get this node's valid actions
    list_t *valid_actions = make_empty_list();
    valid_actions = get_valid_actions(board, player, valid_actions);

    // Traverse through list of valid actions
    node_t *curr = valid_actions->head;
    while (curr) {
        // make a copy of the board
        board_t board_copy;
        copy_board(board, board_copy);

        // perform the current action onto the copy
        do_action(board_copy, curr->action, player);

        // recursive call find the cost of the leaf node
        node_t *child_node = minimax(depth+1, max_depth, board_copy, opp, curr);

        // compare the leaf cost with the best cost
        if ((player == BLACK && (child_node->cost > node->cost))
            || (player == WHITE && (child_node->cost < node->cost))) {
            if (depth == ROOT_DEPTH) {
                // replace root action value with the best action
                strcpy(node->action, child_node->action);
            }
            // pass the child node cost up to the parent node
            node->cost = child_node->cost;
        }
        curr = curr->next;
    }
    free(curr);
    free_list(valid_actions);
    return node;
}

/* ------------------------ Minimax Helper Functions ------------------------ */
/* Find valid moves for the board for the current player and inserts them into
    a linked list.
*/
list_t
*get_valid_actions(board_t board, int player, list_t *valid_actions) {
    // Traversing the board in row-major order
    for (int row=0; row<BOARD_SIZE; row++) {
        for (int col=0; col<BOARD_SIZE; col++) {
            char cell = board[row][col];
            // Finding for cells containing the player's pieces
            if ((player == BLACK && (cell == CELL_BPIECE || 
                                                        cell == CELL_BTOWER))
                || (player == WHITE && (cell == CELL_WPIECE ||
                                                        cell == CELL_WTOWER))
            ) {
                // Check north-east actions
                get_valid_action_in_dir(col, row, NORTH, EAST, player, board,
                    valid_actions);
                // Check south-east actions
                get_valid_action_in_dir(col, row, SOUTH, EAST, player, board,
                    valid_actions);
                // Check south-west actions
                get_valid_action_in_dir(col, row, SOUTH, WEST, player, board,
                    valid_actions);
                // Check north-west actions
                get_valid_action_in_dir(col, row, NORTH, WEST, player, board,
                    valid_actions);
            }
        }
    }
    return valid_actions;
}

/* Adds valid actions in a certain direction for a specific piece into 
    `valid_actions`.
*/
list_t
*get_valid_action_in_dir(int col, int row, int row_diff, int col_diff, 
    int player, board_t board, list_t *valid_actions) {
    // Create an action in this direction
    action_t action;
    action[SRC_COL] = col - COL_OFFSET;
    action[SRC_ROW] = row - ROW_OFFSET;
    action[TAR_COL] = action[SRC_COL] + col_diff;
    action[TAR_ROW] = action[SRC_ROW] + row_diff;
    action[ACTION_LEN] = '\0';
    
    // Check if the action is legal
    if (is_action_legal(board, action, player, FALSE)) {    // legal action
        // ddd it to the linked list
        node_t *action_node = create_new_node(action);
        valid_actions = insert_at_foot(valid_actions, action_node);
    } else if (abs(row_diff) == 1) {                             // illegal move
        // now check for capture in direction
        get_valid_action_in_dir(col, row, CAP_STEP*row_diff, CAP_STEP*col_diff,
            player, board, valid_actions);
    }
    
    return valid_actions;
}

/* Copies all values from `board` to `board_copy`.
*/
void
copy_board(board_t board, board_t board_copy) {
    for (int i=0; i<BOARD_SIZE; i++) {
        for (int j=0; j<BOARD_SIZE; j++) {
            board_copy[i][j] = board[i][j];
        }
    }
}

/* -------------------------- Linked List Functions ------------------------- */

/* Creates and returns a new node.
*/
node_t
*create_new_node(char *action) {
    node_t* new = (node_t*)malloc(sizeof(node_t));
    assert(new!=NULL);
    strcpy(new->action, action);
    new->next = NULL;
    return new;
}

/* Create and returns an empty linked list.
    Adapted version of the make_empty_list function by Alistair Moffat:
    https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
*/
list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

/* Adds an action to the foot-end of `list` and returns the list.
    Adapted version of the insert_at_foot function by Alistair Moffat:
    https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
*/
list_t
*insert_at_foot(list_t *list, node_t *new) {
    assert(list!=NULL);
    if (list->foot == NULL) {  // first action in the list
        list->head = new;
        list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
    return list;
}

/* Free the memory alocated to `list`.
    Adapted version of the free_list function by Alistair Moffat:
    https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
*/
void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

/* THE END -------------------------------------------------------------------*/

/* algorithms are fun */
