
#define NUM_ROWS <%= matrix.row_count %>
#define NUM_COLS <%= matrix.col_count %>

typedef uint8_t Cell;

<% if block_ghost_keys then %>
#define BLOCK_GHOST_KEYS
<% end %>

static inline
void
activate_row(uint8_t row)
{
  // set all row pins as inputs
<% for i,port in ipairs(row_ports) do %>
  DDR<%= string.sub(port,1,1) %> &= ~(1 << <%= port %>);
<% end %>

  // set current row pin as output
  switch (row)
  {
<% for i,port in ipairs(row_ports) do %>
    case <%= i %>: DDR<%= string.sub(row_ports[i],1,1) %> |= (1 << <%= port %>); break;
<% end %>
  }

  // drive all row pins high
<% for i,port in ipairs(row_ports) do %>
  PORT<%= string.sub(port,1,1) %> |= (1 << <%= port %>);
<% end %>

  // drive current row pin low
  switch (row)
  {
<% for i,port in ipairs(row_ports) do %>
    case <%= i %>: PORT<%= string.sub(port,1,1) %> &= ~(1 << <%= port %>); break;
<% end %>
  }
}

static inline
uint32_t
read_row_data(void)
{
  uint32_t cols = 0;

<% for i,port in ipairs(col_ports) do %>
  if ((~PIN<%= string.sub(port,1,1) %>)&(1<<<%= port %>)) cols |= (1UL<< <%= i %>);
<% end %>

  return cols;
}


static inline
void
init_cols(void)
{
  /* Columns are inputs */
<% for i,port in ipairs(col_ports) do %>
  DDR<%= string.sub(port,1,1) %> &= ~(1 << <%= port %>);
<% end %>

  /* Enable pull-up resistors on inputs */
<% for i,port in ipairs(col_ports) do %>
  PORT<%= string.sub(port,1,1) %> |= (1 << <%= port %>);
<% end %>
}

#endif /* __MATRIX_H__ */

