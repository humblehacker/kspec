
#define NUM_ROWS <%= kb.matrix.row_count %>
#define NUM_COLS <%= kb.matrix.col_count %>

typedef uint8_t Cell;

<% if kb.block_ghost_keys then %>
#define BLOCK_GHOST_KEYS
<% end %>

static inline
void
activate_row(uint8_t row)
{
  // set all row pins as inputs
<% for i,pin in ipairs(kb.row_pins) do %>
  DDR<%= string.sub(pin,1,1) %> &= ~(1 << <%= pin %>);
<% end %>

  // set current row pin as output
  switch (row)
  {
<% for i,pin in ipairs(kb.row_pins) do %>
    case <%= i %>: DDR<%= string.sub(pin,1,1) %> |= (1 << <%= pin %>); break;
<% end %>
  }

  // drive all row pins high
<% for i,pin in ipairs(kb.row_pins) do %>
  PORT<%= string.sub(pin,1,1) %> |= (1 << <%= pin %>);
<% end %>

  // drive current row pin low
  switch (row)
  {
<% for i,pin in ipairs(kb.row_pins) do %>
    case <%= i %>: PORT<%= string.sub(pin,1,1) %> &= ~(1 << <%= pin %>); break;
<% end %>
  }
}

static inline
uint32_t
read_row_data(void)
{
  uint32_t cols = 0;

<% for i,pin in ipairs(kb.col_pins) do %>
  if ((~PIN<%= string.sub(pin,1,1) %>)&(1<<<%= pin %>)) cols |= (1UL<< <%= i %>);
<% end %>

  return cols;
}


static inline
void
init_cols(void)
{
  /* Columns are inputs */
<% for i,pin in ipairs(kb.col_pins) do %>
  DDR<%= string.sub(pin,1,1) %> &= ~(1 << <%= pin %>);
<% end %>

  /* Enable pull-up resistors on inputs */
<% for i,pin in ipairs(kb.col_pins) do %>
  PORT<%= string.sub(pin,1,1) %> |= (1 << <%= pin %>);
<% end %>
}

#endif /* __MATRIX_H__ */

