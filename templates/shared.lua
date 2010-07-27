function binding_identifier(keymap, location, premods, class)
    return (keymap.name .. '_' .. location .. '_' .. premods .. '_' .. class)
end

function normalize_identifier(ident)
    return ident
end
