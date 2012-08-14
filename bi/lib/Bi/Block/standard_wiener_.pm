=head1 NAME

standard_wiener_ - optimisation block for standard L<wiener> actions.

=cut

package Bi::Block::standard_wiener_;

use base 'Bi::Model::Block';
use warnings;
use strict;

our $BLOCK_ARGS = [];

sub validate {
    my $self = shift;
    
    $self->process_args($BLOCK_ARGS);
    
    if ($self->num_blocks > 0) {
        die("a 'standard_wiener_' block may not contain sub-blocks\n");
    }

    foreach my $action (@{$self->get_actions}) {
        if ($action->get_name ne 'wiener') {
            die("a 'standard_wiener_' block may only contain 'wiener' actions\n");
        }
    }
}

1;

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$