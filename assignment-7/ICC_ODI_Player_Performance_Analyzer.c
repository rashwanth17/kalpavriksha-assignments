#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Players_data.h"

typedef struct PlayerNode
{
    int player_id;
    char player_name[50];
    char team_name[50];
    char role_name[15];
    int total_runs;
    float batting_average;
    float strike_rate;
    int wickets;
    float economy_rate;
    float performance_index;
    int role_index;
    struct PlayerNode *next_in_team;
    struct PlayerNode *next_in_role;
} PlayerNode;

typedef struct TeamNode
{
    int team_id;
    char team_name[64];
    int total_players;
    float strike_rate_sum;
    int strike_rate_contributors;
    float average_batting_strike_rate;
    PlayerNode *players_head;
    PlayerNode *role_head_batsman;
    PlayerNode *role_head_bowler;
    PlayerNode *role_head_allrounder;
} TeamNode;

static TeamNode *team_list = NULL;
static int total_teams_internal = 0;

void ReadLine(char *buffer,int buffer_size)
{
    if (fgets(buffer, buffer_size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return;
}

int ReadIntegerValue()
{
    char input_buffer[128];
    ReadLine(input_buffer, sizeof(input_buffer));
    return atoi(input_buffer);
}

float ReadFloatValue()
{
    char input_buffer[128];
    ReadLine(input_buffer, sizeof(input_buffer));
    return (float)atof(input_buffer);
}

char *TrimLeadingSpaces(char *text)
{
    char *cursor = text;
    while (*cursor != '\0' && isspace((unsigned char)*cursor))
    {
        cursor++;
    }
    return cursor;
}

int RoleNumberToIndex(int role_number)
{
    if (role_number == 1)
    {
        return 0;
    }
    if (role_number == 2)
    {
        return 1;
    }
    if (role_number == 3)
    {
        return 2;
    }
    return -1;
}

void RoleIndexToName(int role_index,char *out_name,int out_size)
{
    if (role_index == 0)
    {
        strncpy(out_name, "Batsman", out_size - 1);
        out_name[out_size - 1] = '\0';
        return;
    }
    if (role_index == 1)
    {
        strncpy(out_name, "Bowler", out_size - 1);
        out_name[out_size - 1] = '\0';
        return;
    }
    strncpy(out_name, "All-Rounder", out_size - 1);
    out_name[out_size - 1] = '\0';
    return;
}

float CalculatePerformanceIndex(const PlayerNode *player_node)
{
    if (player_node == NULL)
    {
        return 0.0f;
    }

    if (player_node->role_index == 0)
    {
        return (player_node->batting_average * player_node->strike_rate) / 100.0f;
    }

    if (player_node->role_index == 1)
    {
        return (player_node->wickets * 2.0f) + (100.0f - player_node->economy_rate);
    }

    return ((player_node->batting_average * player_node->strike_rate) / 100.0f) + (player_node->wickets * 2.0f);
}

int FindTeamIndexByID(int requested_team_id)
{
    int left = 0;
    int right = total_teams_internal - 1;

    while (left <= right)
    {
        int middle = (left + right) / 2;

        if (team_list[middle].team_id == requested_team_id)
        {
            return middle;
        }

        if (team_list[middle].team_id < requested_team_id)
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }

    return -1;
}

void InsertPlayerIntoTeamList(TeamNode *team_node,PlayerNode *player_node)
{
    if (team_node->players_head == NULL)
    {
        team_node->players_head = player_node;
        player_node->next_in_team = NULL;
    }
    else
    {
        PlayerNode *iterator = team_node->players_head;

        while (iterator->next_in_team != NULL)
        {
            iterator = iterator->next_in_team;
        }

        iterator->next_in_team = player_node;
        player_node->next_in_team = NULL;
    }

    team_node->total_players++;
    return;
}

void InsertPlayerIntoRoleSortedList(TeamNode *team_node,PlayerNode *player_node)
{
    PlayerNode **role_head_pointer = NULL;

    if (player_node->role_index == 0)
    {
        role_head_pointer = &team_node->role_head_batsman;
    }
    else if (player_node->role_index == 1)
    {
        role_head_pointer = &team_node->role_head_bowler;
    }
    else
    {
        role_head_pointer = &team_node->role_head_allrounder;
    }

    if (*role_head_pointer == NULL)
    {
        *role_head_pointer = player_node;
        player_node->next_in_role = NULL;
        return;
    }

    if (player_node->performance_index > (*role_head_pointer)->performance_index)
    {
        player_node->next_in_role = *role_head_pointer;
        *role_head_pointer = player_node;
        return;
    }

    PlayerNode *cursor = *role_head_pointer;

    while (cursor->next_in_role != NULL &&
           cursor->next_in_role->performance_index >= player_node->performance_index)
    {
        cursor = cursor->next_in_role;
    }

    player_node->next_in_role = cursor->next_in_role;
    cursor->next_in_role = player_node;

    return;
}

void UpdateTeamStrikeRateOnInsert(TeamNode *team_node,PlayerNode *player_node)
{
    if (player_node->role_index == 0 || player_node->role_index == 2)
    {
        team_node->strike_rate_sum += player_node->strike_rate;
        team_node->strike_rate_contributors++;
        if (team_node->strike_rate_contributors > 0)
        {
            team_node->average_batting_strike_rate = team_node->strike_rate_sum / (float)team_node->strike_rate_contributors;
        }
    }

    return;
}

void InitializeSystemFromHeader()
{
    total_teams_internal = teamCount;

    team_list = malloc(sizeof(TeamNode) * total_teams_internal);

    if (team_list == NULL)
    {
        printf("ERROR: Memory allocation failed for team list\n");
        exit(1);
    }

    for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
    {
        team_list[team_counter].team_id = team_counter + 1;
        strncpy(team_list[team_counter].team_name, teams[team_counter], sizeof(team_list[team_counter].team_name) - 1);
        team_list[team_counter].team_name[sizeof(team_list[team_counter].team_name) - 1] = '\0';
        team_list[team_counter].total_players = 0;
        team_list[team_counter].strike_rate_sum = 0.0f;
        team_list[team_counter].strike_rate_contributors = 0;
        team_list[team_counter].average_batting_strike_rate = 0.0f;
        team_list[team_counter].players_head = NULL;
        team_list[team_counter].role_head_batsman = NULL;
        team_list[team_counter].role_head_bowler = NULL;
        team_list[team_counter].role_head_allrounder = NULL;
    }

    for (int player_counter = 0; player_counter < playerCount; player_counter++)
    {
        PlayerNode *new_node = malloc(sizeof(PlayerNode));

        if (new_node == NULL)
        {
            printf("ERROR: Memory allocation failed for player node\n");
            exit(1);
        }

        new_node->player_id = players[player_counter].id;
        strncpy(new_node->player_name, players[player_counter].name, sizeof(new_node->player_name) - 1);
        new_node->player_name[sizeof(new_node->player_name) - 1] = '\0';
        strncpy(new_node->team_name, players[player_counter].team, sizeof(new_node->team_name) - 1);
        new_node->team_name[sizeof(new_node->team_name) - 1] = '\0';

        int header_role_index = -1;

        if (players[player_counter].role != NULL && players[player_counter].role[0] != '\0')
        {
            char role_buffer[64];
            strncpy(role_buffer, players[player_counter].role, sizeof(role_buffer) - 1);
            role_buffer[sizeof(role_buffer) - 1] = '\0';
            char *trimmed_role = TrimLeadingSpaces(role_buffer);
            int set_role = -1;
            if (strcasecmp(trimmed_role, "Batsman") == 0)
            {
                set_role = 0;
            }
            else if (strcasecmp(trimmed_role, "Bowler") == 0)
            {
                set_role = 1;
            }
            else
            {
                set_role = 2;
            }
            header_role_index = set_role;
        }
        else
        {
            header_role_index = 0;
        }

        new_node->role_index = header_role_index;

        RoleIndexToName(new_node->role_index, new_node->role_name, sizeof(new_node->role_name));
        new_node->total_runs = players[player_counter].totalRuns;
        new_node->batting_average = players[player_counter].battingAverage;
        new_node->strike_rate = players[player_counter].strikeRate;
        new_node->wickets = players[player_counter].wickets;
        new_node->economy_rate = players[player_counter].economyRate;

        new_node->performance_index = CalculatePerformanceIndex(new_node);
        new_node->next_in_team = NULL;
        new_node->next_in_role = NULL;

        int matching_team_index = -1;

        for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
        {
            if (strcmp(team_list[team_counter].team_name, new_node->team_name) == 0)
            {
                matching_team_index = team_counter;
                break;
            }
        }

        if (matching_team_index == -1)
        {
            free(new_node);
            continue;
        }

        InsertPlayerIntoTeamList(&team_list[matching_team_index], new_node);
        InsertPlayerIntoRoleSortedList(&team_list[matching_team_index], new_node);
        UpdateTeamStrikeRateOnInsert(&team_list[matching_team_index], new_node);
    }

    return;
}

void AddPlayerToTeam()
{
    printf("Enter Team ID to add player: ");
    int requested_team_id = ReadIntegerValue();

    int found_index = FindTeamIndexByID(requested_team_id);

    if (found_index == -1)
    {
        printf("ERROR: Invalid Team ID\n");
        return;
    }

    TeamNode *selected_team = &team_list[found_index];

    PlayerNode *new_node = malloc(sizeof(PlayerNode));

    if (new_node == NULL)
    {
        printf("ERROR: Memory allocation failed\n");
        return;
    }

    printf("Enter Player Details:\n");

    printf("Player ID: ");
    new_node->player_id = ReadIntegerValue();

    printf("Name: ");
    ReadLine(new_node->player_name, sizeof(new_node->player_name));
    char *trimmed_name = TrimLeadingSpaces(new_node->player_name);
    if (trimmed_name != new_node->player_name)
    {
        memmove(new_node->player_name, trimmed_name, strlen(trimmed_name) + 1);
    }

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role_number = ReadIntegerValue();
    int role_index = RoleNumberToIndex(role_number);

    if (role_index < 0)
    {
        printf("ERROR: Invalid role selection\n");
        free(new_node);
        return;
    }

    new_node->role_index = role_index;
    RoleIndexToName(new_node->role_index, new_node->role_name, sizeof(new_node->role_name));

    printf("Total Runs: ");
    new_node->total_runs = ReadIntegerValue();

    printf("Batting Average: ");
    new_node->batting_average = ReadFloatValue();

    printf("Strike Rate: ");
    new_node->strike_rate = ReadFloatValue();

    printf("Wickets: ");
    new_node->wickets = ReadIntegerValue();

    printf("Economy Rate: ");
    new_node->economy_rate = ReadFloatValue();

    strncpy(new_node->team_name, selected_team->team_name, sizeof(new_node->team_name) - 1);
    new_node->team_name[sizeof(new_node->team_name) - 1] = '\0';

    new_node->performance_index = CalculatePerformanceIndex(new_node);
    new_node->next_in_team = NULL;
    new_node->next_in_role = NULL;

    InsertPlayerIntoTeamList(selected_team, new_node);
    InsertPlayerIntoRoleSortedList(selected_team, new_node);
    UpdateTeamStrikeRateOnInsert(selected_team, new_node);

    printf("Player added successfully to Team %s!\n", selected_team->team_name);

    return;
}

void DisplayPlayersOfSpecificTeam()
{
    printf("Choice 2 \u2192 Display All Players of a Specific Team\n");
    printf("Enter Team ID: ");
    int requested_team_id = ReadIntegerValue();

    int found_index = FindTeamIndexByID(requested_team_id);

    if (found_index == -1)
    {
        printf("ERROR: Invalid Team ID\n");
        return;
    }

    TeamNode *selected_team = &team_list[found_index];

    printf("Players of Team %s:\n", selected_team->team_name);
    printf("====================================================================================\n");
    printf("%-6s %-20s %-12s %-6s %-6s %-6s %-6s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER");
    printf("====================================================================================\n");

    PlayerNode *iterator = selected_team->players_head;

    while (iterator != NULL)
    {
        printf("%-6d %-20s %-12s %-6d %-6.1f %-6.1f %-6d %-10.1f\n",
               iterator->player_id,
               iterator->player_name,
               iterator->role_name,
               iterator->total_runs,
               iterator->batting_average,
               iterator->strike_rate,
               iterator->wickets,
               iterator->economy_rate);
        iterator = iterator->next_in_team;
    }

    printf("====================================================================================\n");
    printf("Total Players: %d\n", selected_team->total_players);
    printf("Average Batting Strike Rate: %.2f\n\n", selected_team->average_batting_strike_rate);

    return;
}

int CompareTeamsByAverageStrikeRateDesc(const void *left_element,const void *right_element)
{
    const TeamNode *left_team = (const TeamNode *)left_element;
    const TeamNode *right_team = (const TeamNode *)right_element;

    if (left_team->average_batting_strike_rate < right_team->average_batting_strike_rate)
    {
        return 1;
    }

    if (left_team->average_batting_strike_rate > right_team->average_batting_strike_rate)
    {
        return -1;
    }

    return 0;
}

void DisplayTeamsByAverageBattingStrikeRate()
{
    TeamNode *sorting_array = malloc(sizeof(TeamNode) * total_teams_internal);

    if (sorting_array == NULL)
    {
        printf("ERROR: Memory allocation failed\n");
        return;
    }

    for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
    {
        sorting_array[team_counter] = team_list[team_counter];
    }

    qsort(sorting_array, total_teams_internal, sizeof(TeamNode), CompareTeamsByAverageStrikeRateDesc);

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-4s %-18s %-12s %-8s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");

    for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
    {
        printf("%-4d %-18s %-12.1f %-8d\n",
               sorting_array[team_counter].team_id,
               sorting_array[team_counter].team_name,
               sorting_array[team_counter].average_batting_strike_rate,
               sorting_array[team_counter].total_players);
    }

    printf("=========================================================\n\n");

    free(sorting_array);

    return;
}

void DisplayTopKPlayersOfTeamByRole()
{
    printf("Choice 4 \u2192 Display Top K Players of a Specific Team by Role\n");
    printf("Enter Team ID: ");
    int requested_team_id = ReadIntegerValue();

    int found_index = FindTeamIndexByID(requested_team_id);

    if (found_index == -1)
    {
        printf("ERROR: Invalid Team ID\n");
        return;
    }

    TeamNode *selected_team = &team_list[found_index];

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role_number = ReadIntegerValue();
    int role_index = RoleNumberToIndex(role_number);

    if (role_index < 0)
    {
        printf("ERROR: Invalid role selection\n");
        return;
    }

    printf("Enter number of players: ");
    int requested_k = ReadIntegerValue();

    if (requested_k <= 0)
    {
        printf("ERROR: K must be positive\n");
        return;
    }

    PlayerNode *role_iterator = NULL;

    if (role_index == 0)
    {
        role_iterator = selected_team->role_head_batsman;
    }
    else if (role_index == 1)
    {
        role_iterator = selected_team->role_head_bowler;
    }
    else
    {
        role_iterator = selected_team->role_head_allrounder;
    }

    if (role_iterator == NULL)
    {
        printf("No players of the specified role in this team\n");
        return;
    }

    char role_name_print[32];
    RoleIndexToName(role_index, role_name_print, sizeof(role_name_print));

    printf("Top %d %s(s) of Team %s:\n", requested_k, role_name_print, selected_team->team_name);
    printf("====================================================================================\n");
    printf("%-6s %-20s %-12s %-6s %-6s %-6s %-6s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "Perf.Index");
    printf("====================================================================================\n");

    int printed = 0;

    while (role_iterator != NULL && printed < requested_k)
    {
        printf("%-6d %-20s %-12s %-6d %-6.1f %-6.1f %-6d %-10.2f\n",role_iterator->player_id,role_iterator->player_name,role_iterator->role_name,role_iterator->total_runs,role_iterator->batting_average,role_iterator->strike_rate,role_iterator->wickets,role_iterator->performance_index);
        role_iterator = role_iterator->next_in_role;
        printed++;
    }

    printf("====================================================================================\n\n");

    return;
}

typedef struct HeapNode
{
    PlayerNode *player_node;
    int originating_team_index;
} HeapNode;

void SwapHeapNodes(HeapNode *a,HeapNode *b)
{
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
    return;
}

void PushHeap(HeapNode heap_array[],int *heap_size_pointer,HeapNode new_element)
{
    int insert_index = (*heap_size_pointer);
    heap_array[insert_index] = new_element;
    (*heap_size_pointer)++;

    int current_index = insert_index;

    while (current_index > 0)
    {
        int parent_index = (current_index - 1) / 2;

        if (heap_array[parent_index].player_node->performance_index >= heap_array[current_index].player_node->performance_index)
        {
            break;
        }

        SwapHeapNodes(&heap_array[parent_index], &heap_array[current_index]);
        current_index = parent_index;
    }

    return;
}

HeapNode PopHeap(HeapNode heap_array[],int *heap_size_pointer)
{
    HeapNode top = heap_array[0];
    (*heap_size_pointer)--;
    heap_array[0] = heap_array[*heap_size_pointer];

    int current_index = 0;

    while (1)
    {
        int left_child = 2 * current_index + 1;
        int right_child = 2 * current_index + 2;
        int largest = current_index;

        if (left_child < *heap_size_pointer &&
            heap_array[left_child].player_node->performance_index > heap_array[largest].player_node->performance_index)
        {
            largest = left_child;
        }

        if (right_child < *heap_size_pointer &&
            heap_array[right_child].player_node->performance_index > heap_array[largest].player_node->performance_index)
        {
            largest = right_child;
        }

        if (largest == current_index)
        {
            break;
        }

        SwapHeapNodes(&heap_array[current_index], &heap_array[largest]);
        current_index = largest;
    }

    return top;
}

void DisplayAllPlayersAcrossTeamsByRole()
{
    printf("Choice 5 \u2192 Display All Players Across All Teams of specific role\n");
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role_number = ReadIntegerValue();
    int role_index = RoleNumberToIndex(role_number);

    if (role_index < 0)
    {
        printf("ERROR: Invalid role selection\n");
        return;
    }

    HeapNode *heap_array = malloc(sizeof(HeapNode) * total_teams_internal);

    if (heap_array == NULL)
    {
        printf("ERROR: Memory allocation failed\n");
        return;
    }

    int heap_size = 0;

    for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
    {
        PlayerNode *head_for_role = NULL;

        if (role_index == 0)
        {
            head_for_role = team_list[team_counter].role_head_batsman;
        }
        else if (role_index == 1)
        {
            head_for_role = team_list[team_counter].role_head_bowler;
        }
        else
        {
            head_for_role = team_list[team_counter].role_head_allrounder;
        }

        if (head_for_role != NULL)
        {
            HeapNode element;
            element.player_node = head_for_role;
            element.originating_team_index = team_counter;
            PushHeap(heap_array, &heap_size, element);
        }
    }

    if (heap_size == 0)
    {
        printf("No players of that role across all teams\n");
        free(heap_array);
        return;
    }

    char role_name_print[32];
    RoleIndexToName(role_index, role_name_print, sizeof(role_name_print));

    printf("\n%s of all teams:\n", role_name_print);
    printf("======================================================================================\n");
    printf("%-6s %-20s %-16s %-16s %-6s %-6s %-6s %-10s\n","ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Perf.Index");
    printf("======================================================================================\n");

    while (heap_size > 0)
    {
        HeapNode top = PopHeap(heap_array, &heap_size);
        PlayerNode *node = top.player_node;
        printf("%-6d %-20s %-16s %-16s %-6d %-6.1f %-6.1f %-10.2f\n",node->player_id,node->player_name,node->team_name,node->role_name,node->total_runs,node->batting_average,node->strike_rate,node->performance_index);

        PlayerNode *next_in_that_team = node->next_in_role;

        if (next_in_that_team != NULL)
        {
            HeapNode new_element;
            new_element.player_node = next_in_that_team;
            new_element.originating_team_index = top.originating_team_index;
            PushHeap(heap_array, &heap_size, new_element);
        }
    }

    printf("======================================================================================\n\n");

    free(heap_array);

    return;
}

void FreeAllSystemMemory()
{
    if (team_list == NULL)
    {
        return;
    }

    for (int team_counter = 0; team_counter < total_teams_internal; team_counter++)
    {
        PlayerNode *iterator = team_list[team_counter].players_head;

        while (iterator != NULL)
        {
            PlayerNode *next_node = iterator->next_in_team;
            free(iterator);
            iterator = next_node;
        }
    }

    free(team_list);
    team_list = NULL;
    total_teams_internal = 0;

    return;
}

int main()
{
    printf("==============================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    InitializeSystemFromHeader();

    while (1)
    {
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");
        printf("Enter your choice: ");

        int choice = ReadIntegerValue();

        if (choice == 1)
        {
            AddPlayerToTeam();
        }
        else if (choice == 2)
        {
            DisplayPlayersOfSpecificTeam();
        }
        else if (choice == 3)
        {
            DisplayTeamsByAverageBattingStrikeRate();
        }
        else if (choice == 4)
        {
            DisplayTopKPlayersOfTeamByRole();
        }
        else if (choice == 5)
        {
            DisplayAllPlayersAcrossTeamsByRole();
        }
        else if (choice == 6)
        {
            FreeAllSystemMemory();
            printf("Goodbye\n");
            break;
        }
        else
        {
            printf("ERROR: Invalid choice\n");
        }
    }

    return 0;
}
